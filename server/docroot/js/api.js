/*
 * SPDX-FileCopyrightText: 2018-2020 Harald Gültig <hgueltig@gmail.com>
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

var ws;
var cbOnStart_, cbOnMessage_, cbOnDisconnected_;

function WsSetup(cbOnStart, cbOnMessage, cbOnDisconnected) {
	// cbOnMessage = function(msgType, msgData)
	cbOnStart_ = cbOnStart;
	cbOnMessage_ = cbOnMessage;
	cbOnDisconnected_ = cbOnDisconnected;

	connect();
};

function connect() {
	if (!ws || ws.readyState == ws.CLOSED) {
		const is_ssl = (location.protocol === 'https:');

		ws = new WebSocket((is_ssl ? "wss" : "ws") + "://" + window.location.hostname + ":8888/");
		console.log("Setting up WebSockets");

		ws.onopen = function () {
			console.log("Connection etablished");
			cbOnStart_();
		};
		ws.onmessage = function (evt) {
			var message = JSON.parse(evt.data);
			cbOnMessage_(message.type, message.data);
		};
		ws.onerror = function (errorEvent) {
			cbOnDisconnected_("Die Verbindung wurde unerwartet geschlossen");
			setTimeout(connect, 3000);
		};
		ws.onclose = function () {
			cbOnDisconnected_("Connection closed");
			setTimeout(connect, 3000);
		};
	}
}

function api(query, data) {
	console.log("Request", JSON.stringify({ request: query, data: data }));
	ws.send(JSON.stringify({ request: query, data: data }));
}