import struct
import os

def pack_timestamp(nanos):
    # Pack into 6 bytes, big endian
    return struct.pack(">Q", nanos)[2:8]

def pad_str(s, length):
    return s[:length].ljust(length, ' ').encode('ascii')

def generate_add_order(stock_locate, tracking, ts, ref, side, shares, stock, price):
    # 'A' c H H 6s Q c I 8s I
    return struct.pack(">cHH", b'A', stock_locate, tracking) + \
           pack_timestamp(ts) + \
           struct.pack(">QcI8sI", ref, side.encode('ascii'), shares, pad_str(stock, 8), price)

def generate_add_order_mpid(stock_locate, tracking, ts, ref, side, shares, stock, price, mpid):
    # 'F' c H H 6s Q c I 8s I 4s
    return struct.pack(">cHH", b'F', stock_locate, tracking) + \
           pack_timestamp(ts) + \
           struct.pack(">QcI8sI4s", ref, side.encode('ascii'), shares, pad_str(stock, 8), price, pad_str(mpid, 4))

def generate_order_executed(stock_locate, tracking, ts, ref, executed_shares, match_number):
    # 'E' c H H 6s Q I Q
    return struct.pack(">cHH", b'E', stock_locate, tracking) + \
           pack_timestamp(ts) + \
           struct.pack(">QIQ", ref, executed_shares, match_number)

def generate_order_executed_with_price(stock_locate, tracking, ts, ref, executed_shares, match_number, printable, execution_price):
    # 'C' c H H 6s Q I Q c I
    return struct.pack(">cHH", b'C', stock_locate, tracking) + \
           pack_timestamp(ts) + \
           struct.pack(">QIQcI", ref, executed_shares, match_number, printable.encode('ascii'), execution_price)

def generate_order_cancel(stock_locate, tracking, ts, ref, canceled_shares):
    # 'X' c H H 6s Q I
    return struct.pack(">cHH", b'X', stock_locate, tracking) + \
           pack_timestamp(ts) + \
           struct.pack(">QI", ref, canceled_shares)

def generate_order_delete(stock_locate, tracking, ts, ref):
    # 'D' c H H 6s Q
    return struct.pack(">cHH", b'D', stock_locate, tracking) + \
           pack_timestamp(ts) + \
           struct.pack(">Q", ref)

def generate_order_replace(stock_locate, tracking, ts, orig_ref, new_ref, shares, price):
    # 'U' c H H 6s Q Q I I
    return struct.pack(">cHH", b'U', stock_locate, tracking) + \
           pack_timestamp(ts) + \
           struct.pack(">QQII", orig_ref, new_ref, shares, price)

def frame_message(msg_bytes):
    # Prefix message with a 2-byte big-endian length (SoupBinTCP style)
    return struct.pack(">H", len(msg_bytes)) + msg_bytes

def main():
    filename = 'test_data.itch'
    with open(filename, 'wb') as f:
        # Example 1: Add Order (A)
        f.write(frame_message(generate_add_order(
            stock_locate=1, tracking=1, ts=34200000000000, 
            ref=10001, side='B', shares=100, stock='AAPL', price=1500000
        )))
        
        # Example 2: Add Order with MPID (F)
        f.write(frame_message(generate_add_order_mpid(
            stock_locate=1, tracking=2, ts=34200001000000, 
            ref=10002, side='S', shares=200, stock='AAPL', price=1510000, mpid='NSDQ'
        )))
        
        # Example 3: Order Executed (E)
        f.write(frame_message(generate_order_executed(
            stock_locate=1, tracking=3, ts=34200002000000, 
            ref=10001, executed_shares=50, match_number=50001
        )))
        
        # Example 4: Order Executed with Price (C)
        f.write(frame_message(generate_order_executed_with_price(
            stock_locate=1, tracking=4, ts=34200003000000, 
            ref=10002, executed_shares=100, match_number=50002, printable='Y', execution_price=1505000
        )))
        
        # Example 5: Order Cancel (X)
        f.write(frame_message(generate_order_cancel(
            stock_locate=1, tracking=5, ts=34200004000000, 
            ref=10001, canceled_shares=50
        )))
        
        # Example 6: Order Delete (D)
        f.write(frame_message(generate_order_delete(
            stock_locate=1, tracking=6, ts=34200005000000, 
            ref=10002
        )))
        
        # Example 7: Order Replace (U)
        f.write(frame_message(generate_order_replace(
            stock_locate=1, tracking=7, ts=34200006000000, 
            orig_ref=10001, new_ref=10003, shares=200, price=1501000
        )))

    print(f"Successfully generated {filename} with simulated ITCH 5.0 data.")

if __name__ == '__main__':
    main()
