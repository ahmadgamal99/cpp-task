#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using json = nlohmann::json;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// Handlers
void on_open(client* c, websocketpp::connection_hdl hdl) {
    std::string msg = "btcusdt";
    c->send(hdl,msg,websocketpp::frame::opcode::text);
    c->get_alog().write(websocketpp::log::alevel::app, "Sent Message: "+msg);
}

void on_fail(client* c, websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Failed");
}

void storeBinanceData( std::string data ){

    auto j = json::parse(data);

    // Create and open a text file
    std::ofstream MyFile("../binance_order_book.txt",  std::ios::app);

    // Write to the file
    std::string recordData;
    std::string labels[] = {"Best ask qty", "Best bid qty", "Best ask price","Best bid price", "Symbol","id"};

    unsigned i = 0;
    std::cout << j << std::endl;

    for (auto it = j.begin(); it != j.end(); ++it,i++)
    {
        recordData += labels[i] + " : " + to_string(it.value()) + "\n";
    }

    MyFile << recordData + "\n--------------------------------------------------------------------\n";

    MyFile.close();

}

void storeBitstampData( std::string data ){

    auto j = json::parse(data);

    // Create and open a text file
    std::ofstream MyFile("../bitstamp_order_book.txt", std::ios::app);

    // Write to the file

    std::string recordData;
    std::string labels[] = {"Best ask price","Best ask qty","Best bid price","Best bid qty", "Timestamp"};

    unsigned i = 0;

    for (auto it = j.begin(); it != j.end(); ++it,i++)
    {
        recordData += labels[i] + " : " + to_string(it.value()) + "\n";
    }

    MyFile << recordData + "\n--------------------------------------------------------------------\n";

    MyFile.close();

}

void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg, std::string exhange) {
//    c->get_alog().write(websocketpp::log::alevel::app, "Received from " + exhange + " " + msg->get_payload());
//    c->get_alog().write(websocketpp::log::alevel::app, "\n-------------------------------------------\n");


    if ( exhange == "binance")
        storeBinanceData( msg->get_payload() );
    else
        storeBitstampData( msg->get_payload() );
}

void on_close(client* c, websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Closed");
}

void getOrderBook(std::string exchangeUri, std::string exchange)
{
    client c;

    try {
        // set logging policy if needed
        c.clear_access_channels(websocketpp::log::alevel::frame_header);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c.init_asio();

        // Register our handlers
        c.set_open_handler(bind(&on_open,&c,::_1));
        c.set_fail_handler(bind(&on_fail,&c,::_1));
        c.set_message_handler(bind(&on_message,&c,::_1,::_2, exchange));
        c.set_close_handler(bind(&on_close,&c,::_1));

        // Create a connection to the given URI and queue it for connection once
        // the event loop starts
        websocketpp::lib::error_code ec;
        client::connection_ptr con  = c.get_connection(exchangeUri, ec);
        c.connect(con);

        // Start the ASIO io_service run loop
        c.run();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    } catch (websocketpp::lib::error_code e) {
        std::cout << e.message() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}

int main(int argc, char* argv[]) {

    std::string binanceUri = "ws://localhost:3000"; // binance
    std::string bitstampUri = "ws://localhost:3001"; // bitstamp

    std::thread binanceThread(getOrderBook, binanceUri, "binance");
    std::thread bitstampThread(getOrderBook, bitstampUri, "bitstamp");

    binanceThread.join();
    bitstampThread.join();
}

