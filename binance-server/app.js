"use strict";

import http from "http";
import https from "https";
import fs from "fs";
import WebSocket, {WebSocketServer} from 'ws';


const localServer = http.createServer();

const binanceServer = https.createServer({
    cert: fs.readFileSync('certificate/dafa.pem'),
    key: fs.readFileSync('certificate/dafa.key')
});


const localWs = new WebSocketServer({server: localServer});
let client;

binanceServer.listen(function listening() {

    /** get stream from binance **/

    const binanceWs = new WebSocket("wss://stream.binance.com:9443/ws", {
        rejectUnauthorized: true,
    });

    binanceWs.on('error', console.error);

    binanceWs.on('open', function open() {

        const msg = {
            method: 'SUBSCRIBE',
            params: ['btcusdt@bookTicker'],
            id: 1,
        };

        binanceWs.send(JSON.stringify(msg));

    });


    binanceWs.on('message', function message(data, isBinary) {
        const message = isBinary ? data : data.toString();
        console.log("data returned", message);

        if (client && message)
            client.send(message)
    });

    binanceWs.on('error', console.error);

    /** get stream from binance **/

});

/** send stream to cpp app **/
localWs.on('connection', function (ws) {

    client = ws;

    console.log('new node has connected');

    ws.on('message', function (data) {
        console.log('>>> ' + data);
    });

    ws.on('close', function () {
        console.log('Connection closed!');
    });

    ws.on('error', function (e) {
    });
});
/** send stream to cpp app **/


const PORT = 3000;
localServer.listen(PORT);
console.log(`Listening on port ${PORT}...`);
