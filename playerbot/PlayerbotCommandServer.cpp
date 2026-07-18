
#include "playerbot/playerbot.h"
#include "playerbot/PlayerbotAIConfig.h"
#include "playerbot/PlayerbotFactory.h"
#include "PlayerbotCommandServer.h"
#include <cstdlib>
#include <iostream>

INSTANTIATE_SINGLETON_1(PlayerbotCommandServer);

#include <boost/bind/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#ifdef VMANGOS
#include <thread> // boost::thread would autolink a boost library vmangos doesn't ship
#include <mutex>
#include <future>
#include <deque>
#include <chrono>
#else
#include <boost/thread/thread.hpp>
#endif

using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> socket_ptr;

#ifdef VMANGOS
// HandleRemoteCommand touches bot/world state and must not run on the
// per-connection threads (racing the world/map update froze map updates).
// Connection threads enqueue requests here; the world thread answers them
// via ProcessQueuedCommands() (called from RandomPlayerbotMgr::UpdateAIInternal).
namespace
{
    struct PendingRemoteCommand
    {
        std::string request;
        std::promise<std::string> reply;
    };

    std::mutex s_remoteCmdMutex;
    std::deque<std::shared_ptr<PendingRemoteCommand>> s_remoteCmdQueue;

    std::string QueueRemoteCommand(std::string const& request, uint32 timeoutMs)
    {
        auto cmd = std::make_shared<PendingRemoteCommand>();
        cmd->request = request;
        std::future<std::string> fut = cmd->reply.get_future();
        {
            std::lock_guard<std::mutex> lock(s_remoteCmdMutex);
            s_remoteCmdQueue.push_back(cmd);
        }
        if (fut.wait_for(std::chrono::milliseconds(timeoutMs)) == std::future_status::ready)
            return fut.get();
        return "timeout";
    }
}

void PlayerbotCommandServer::ProcessQueuedCommands()
{
    std::deque<std::shared_ptr<PendingRemoteCommand>> batch;
    {
        std::lock_guard<std::mutex> lock(s_remoteCmdMutex);
        batch.swap(s_remoteCmdQueue);
    }
    for (auto& cmd : batch)
        cmd->reply.set_value(sRandomPlayerbotMgr.HandleRemoteCommand(cmd->request));
}
#endif

bool ReadLine(socket_ptr sock, std::string* buffer, std::string* line)
{
    // Do the real reading from fd until buffer has '\n'.
    std::string::iterator pos;
    while ((pos = find(buffer->begin(), buffer->end(), '\n')) == buffer->end())
    {
        char buf[1025];
        boost::system::error_code error;
        size_t n = sock->read_some(boost::asio::buffer(buf), error);
        if (n == -1 || error == boost::asio::error::eof)
            return false;
        else if (error)
            throw boost::system::system_error(error); // Some other error.

        buf[n] = 0;
        *buffer += buf;
    }

    *line = std::string(buffer->begin(), pos);
    *buffer = std::string(pos + 1, buffer->end());
    return true;
}

void session(socket_ptr sock)
{
    try
    {
        std::string buffer, request;
        while (ReadLine(sock, &buffer, &request)) {
#ifdef VMANGOS
            // Answered on the world thread (see QueueRemoteCommand above).
            std::string response = QueueRemoteCommand(request, 2500) + "\n";
#else
            std::string response = sRandomPlayerbotMgr.HandleRemoteCommand(request) + "\n";
#endif
            boost::asio::write(*sock, boost::asio::buffer(response.c_str(), response.size()));
            request = "";
        }
    }
    catch (std::exception& e)
    {
        sLog.outError("%s",e.what());
    }
}

void server(boost::asio::io_context& io_context, short port)
{
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        socket_ptr sock(new tcp::socket(io_context));
        a.accept(*sock);
#ifdef VMANGOS
        std::thread(session, sock).detach();
#else
        boost::thread t(boost::bind(session, sock));
#endif
    }
}

void Run()
{
    if (!sPlayerbotAIConfig.commandServerPort) {
        return;
    }

    std::ostringstream s; s << "Starting Playerbot Command Server on port " << sPlayerbotAIConfig.commandServerPort;
    sLog.outString("%s",s.str().c_str());

    try
    {
        boost::asio::io_context io_context;
        server(io_context, sPlayerbotAIConfig.commandServerPort);
    }
    catch (std::exception& e)
    {
        sLog.outError("%s",e.what());
    }
}

void PlayerbotCommandServer::Start()
{
    std::thread serverThread(Run);
    serverThread.detach();
}
