"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var host = "localhost";
var protocol = "http";
var port = 9200;
// Create a client
var { Client } = require("@opensearch-project/opensearch");
var client = new Client({
    node: protocol + "://" + host + ":" + port
});
var query = {
    query: {
        match: {
            title: {
                query: "The Outsider",
            },
        },
    },
};
var response = await client.search({
    index: "index_name",
    body: query,
});
//# sourceMappingURL=app.js.map