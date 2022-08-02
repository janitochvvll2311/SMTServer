const orders = [
    {
        id: "SZ1000",
        channel: 0,
        lower: 0,
        upper: 2,
        a: 0,
        r: 0,
        g: 0,
        b: 255,
        clear: false,
        pick: false
    },
    {
        id: "SZ1010",
        channel: 0,
        lower: 5,
        upper: 7,
        a: 0,
        r: 0,
        g: 0,
        b: 255,
        clear: false,
        pick: false
    },
    {
        id: "SZ1020",
        channel: 0,
        lower: 10,
        upper: 12,
        a: 0,
        r: 0,
        g: 0,
        b: 255,
        clear: false,
        pick: false
    },
    {
        id: "SZ1030",
        channel: 0,
        lower: 15,
        upper: 17,
        a: 0,
        r: 0,
        g: 0,
        b: 255,
        clear: false,
        pick: false
    }
]

function toggle() {
    const scanner = document.getElementById("scanner");
    if (scanner.value == "ORDEN") {
        orders.forEach((order) => {
            const content = JSON.stringify(order);
            scanner.value = "";
            if (!order.pick) {
                order.pick = true;
                fetch("http://192.168.43.20/api/v1/leds/setpixelrange", {
                    method: "POST",
                    mode: "no-cors",
                    headers: {
                        "Content-Type": "application/json"
                    },
                    body: content
                }).then((response) => {
                    console.log("It Works");
                }).catch((reason) => {
                    console.log(reason);
                });
            } else {
                order.pick = false;
                fetch("http://192.168.43.20/api/v1/leds/clearpixelrange", {
                    method: "POST",
                    mode: "no-cors",
                    headers: {
                        "Content-Type": "application/json"
                    },
                    body: content
                }).then((response) => {
                    console.log("It Works");
                }).catch((reason) => {
                    console.log(reason);
                });
            }
        });
        scanner.value = "";
        return;
    }
    const order = orders.find((v) => v.id == scanner.value);
    const content = JSON.stringify(order);
    scanner.value = "";
    if (!order.pick) {
        order.pick = true;
        fetch("http://192.168.43.20/api/v1/leds/setpixelrange", {
            method: "POST",
            mode: "no-cors",
            headers: {
                "Content-Type": "application/json"
            },
            body: content
        }).then((response) => {
            console.log("It Works");
        }).catch((reason) => {
            console.log(reason);
        });
    } else {
        order.pick = false;
        fetch("http://192.168.43.20/api/v1/leds/clearpixelrange", {
            method: "POST",
            mode: "no-cors",
            headers: {
                "Content-Type": "application/json"
            },
            body: content
        }).then((response) => {
            console.log("It Works");
        }).catch((reason) => {
            console.log(reason);
        });
    }
}