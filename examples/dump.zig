const std = @import("std");
const SkipList = @import("skiplist.zig").EasySkipList(u64, u64, compare);
const print = std.debug.print;

fn compare(a: u64, b: u64) std.math.Order {
    return std.math.order(a, b);
}

fn dump(list: SkipList) void {
    var lvl: usize = list.level - 1;

    while (true) : (lvl -= 1) {
        print("level: {}: ", .{lvl});
        var maybe_node = list.header.forward[lvl];
        while (maybe_node) |node| {
            print("{}[{}]->", .{ node.key, node.value });
            maybe_node = node.forward[lvl];
        }
        print("null\n", .{});

        if (lvl == 0) break;
    }
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    const keys = [_]u64{ 32, 47, 36, 51, 13, 62, 43, 39 };
    const values = [_]u64{ 14, 10, 11, 29, 22, 18, 16, 38 };
    var list = try SkipList.init(allocator, @intCast(std.time.microTimestamp()));
    defer list.deinit();

    for (keys, values) |key, val| {
        _ = try list.insert(key, val);
    }

    dump(list);
}
