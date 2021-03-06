let net = require("net");
let uuid = require("uuid");
let mysql = require("./utils/sql/mysql");

// 创建 TCP 监听实例
net.createServer((socket) => {

    console.log('connected: ' + socket.remoteAddress + ':' + socket.remotePort);

    // 绑定连接事件
    socket.on('connection', (client) => {
        console.log(client + "已连接");
    });

    // 绑定数据事件
    socket.on('data', (data) => {

        /*
            data 数据格式 Wxxxx=xddddddd
                用户码: W0.00=oF7VY5IGuEFW5yD-U4tiXFC8po5A
                商品码: W0.00=c12345678
         */
        console.log(data.toString());
        let s = data.toString().split("=");
        let w = s[0].substring(1,s[0].length) * 1;
        let id = s[1];

        // 商品码，加入当前用户的购物车
	
        if (id[0] == 'c'){
            console.log(id[0]);
            let openid = 'oF7VY5IGuEFW5yD-U4tiXFC8po5A';
            let commodity_id = id.substring(1, id.length).trim();
            console.log(commodity_id);
            mysql.connection.query("select cart from cart where openid = 'oF7VY5IGuEFW5yD-U4tiXFC8po5A'",[],(err, result) => {
                let sql = "INSERT INTO purchased (commodity_id, cart, number) VALUES (?,?,1)";
                let rescart = result[result.length-1].cart;
                console.log(rescart);
                mysql.connection.query(sql, [commodity_id, rescart], (err, result) =>{
                    socket.write("=r"); // 返回黄灯，表示加入购物车成功
                    //console.log(result[0].cart);
                })
            });

        }
        // 用户码，生成购物车id
        else {
	    // socket.write("=g");
            let cart_id = uuid.v4();
            let openid = id.trim();
            console.log(openid);
            let sql = "INSERT INTO cart (openid, cart) VALUES (?,?)";
            mysql.connection.query(sql, [openid, cart_id], (err, result) => {
		console.log("return before...")
                socket.write("=g"); // 返回绑定成功，绿灯
		console.log("return after...")
            });
        }

    });

    // 绑定关闭事件
    socket.on('close', function(data) {});

}).listen(5001);
