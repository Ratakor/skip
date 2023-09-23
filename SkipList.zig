const std = @import("std");

const Allocator = std.mem.Allocator;
const DefaultPrng = std.rand.DefaultPrng;
const Error = error{OutOfMemory};
const MAXLVL = 32;

pub fn SkipList(comptime T: type) type {
    return struct {
        pub const Node = struct {
            key: u64,
            value: T,
            forward: []?*Node,
            backward: ?*Node,

            pub inline fn next(self: Node) ?*Node {
                return self.forward[0];
            }

            pub inline fn prev(self: Node) ?*Node {
                return self.backward;
            }
        };

        const Self = @This();

        header: *Node,
        count: usize = 0,
        level: usize = 1,

        allocator: Allocator,
        prng: DefaultPrng,

        pub fn init(allocator: Allocator, seed: u64) Error!Self {
            const node = try allocator.create(Node);
            node.forward = try allocator.alloc(?*Node, MAXLVL);
            @memset(node.forward, null);
            return Self{
                .header = node,
                .allocator = allocator,
                .prng = DefaultPrng.init(seed),
            };
        }

        pub fn deinit(self: Self) void {
            var node: ?*Node = self.header;
            while (node) |n| {
                node = n.next();
                self.allocator.free(n.forward);
                self.allocator.destroy(n);
            }
        }

        fn find(self: *Self, key: u64, update: ?[]*Node) ?*Node {
            var node: *Node = self.header;
            var lvl = self.level - 1;

            while (true) : (lvl -= 1) {
                while (node.forward[lvl]) |next| {
                    if (next.key >= key) break;
                    node = next;
                }
                if (update) |u| u[lvl] = node;

                if (lvl == 0) break;
            }

            return node.next();
        }

        pub fn insert(self: *Self, key: u64, val: T) Error!*Node {
            var update: [MAXLVL]*Node = undefined;
            const maybe_node = self.find(key, &update);

            if (maybe_node) |node| {
                if (node.key == key) {
                    node.value = val;
                    return node;
                }
            }

            var lvl: usize = 1;
            while (self.prng.random().int(u2) == 0) lvl += 1;
            if (lvl > self.level) {
                if (lvl > MAXLVL) lvl = MAXLVL;
                for (self.level..lvl) |i| {
                    update[i] = self.header;
                }
                self.level = lvl;
            }

            var node = try self.allocator.create(Node);
            node.key = key;
            node.value = val;
            node.forward = try self.allocator.alloc(?*Node, lvl);

            for (0..lvl) |i| {
                node.forward[i] = update[i].forward[i];
                update[i].forward[i] = node;
            }

            node.backward = if (update[0] == self.header) null else update[0];

            if (node.next()) |next| {
                next.backward = node;
            } else {
                self.header.backward = node;
            }

            self.count += 1;

            return node;
        }

        fn delnode(self: *Self, node: *Node, update: []*Node) void {
            for (0..self.level) |lvl| {
                if (update[lvl].forward[lvl] != node) break;
                update[lvl].forward[lvl] = node.forward[lvl];
            }

            while (self.level > 1) {
                if (self.header.forward[self.level - 1] != null) break;

                self.level -= 1;
            }

            if (node.next()) |next| {
                next.backward = node.backward;
            } else {
                self.header.backward = node.backward;
            }

            self.count -= 1;
            self.allocator.free(node.forward);
            self.allocator.destroy(node);
        }

        pub fn delete(self: *Self, key: u64) bool {
            var update: [MAXLVL]*Node = undefined;
            const maybe_node = self.find(key, &update);

            if (maybe_node) |node| {
                if (node.key == key) {
                    self.delnode(node, &update);
                    return true;
                }
            }
            return false;
        }

        pub fn delrange(self: *Self, from: u64, to: u64) usize {
            var update: [MAXLVL]*Node = undefined;
            var maybe_node = self.find(from, &update);
            var total: usize = 0;

            while (maybe_node) |node| {
                if (node.key >= to) break;

                maybe_node = node.next();
                self.delnode(node, &update);
                total += 1;
            }

            return total;
        }

        pub fn search(self: *Self, key: u64) ?*Node {
            const maybe_node = self.find(key, null);
            if (maybe_node) |node| {
                if (node.key == key) {
                    return node;
                }
            }
            return null;
        }

        pub inline fn first(self: Self) ?*Node {
            return self.header.forward[0];
        }

        pub inline fn last(self: Self) ?*Node {
            return self.header.backward;
        }
    };
}
