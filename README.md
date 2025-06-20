# ItchParser

![CI](https://github.com/LukeFasanello/ItchParser/actions/workflows/ci.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

Parses trades from a NASDAQ ITCH 5.0 tick data file to maintain an order book and output
a running volume-weighted average price (VWAP) for each stock 
at every hour including market close.

Message format is defined by the following document: https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf

Messages Processed:
- System Event Message
- Add Order – No MPID Attribution
- Add Order with MPID Attribution
- Order Executed
- Order Executed with Price
- Order Cancel
- Order Delete
- Order Replace
- Trade (Non-Cross)
- Cross Trade
- Broken Trade / Order Execution
