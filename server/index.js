// const cors = require("cors");
const WebSocket = require("ws");
const http = require("http");
const axios = require("axios");

// WEATHER API
const BASE_URL = "http://api.weatherapi.com/v1";

const server = http.createServer((req, res) => {
    const clientIp = "103.187.99.178";

    const WEATHER_API_KEY =
        process.env.WEATHER_API_KEY || "b6725666c4c04e02809103938252001";
    console.log("Weather API Key: ", WEATHER_API_KEY);
    console.log("Client IP: ", clientIp);

    res.writeHead(200, { "Content-Type": "text/plain" });
    res.end("okay");
});

server.listen(5000, () => {
    console.log("Server running at 5000");
});

// Create a WebSocket server
const wss = new WebSocket.Server({ port: 8000 }, () => {
    console.log("WebSocket server started on port 8000");
});
const wss2 = new WebSocket.Server(
    { port: 8001 },
    {
        cors: {
            origin: "*",
        },
    }
);

wss.on("connection", (ws, req) => {
    console.log("NodeMCU connected");
    const clientIp = req.socket.remoteAddress.replace(/^::ffff:/, "");
    const WEATHER_API_KEY =
        process.env.WEATHER_API_KEY || "b6725666c4c04e02809103938252001";
    console.log("Weather API Key: ", WEATHER_API_KEY);
    console.log("Client IP: ", clientIp);
    let lastWeatherCheck = new Date();

    // Receive data from NodeMCU
    ws.on("message", (message, isBinary) => {
        if (isBinary) {
            console.log("Binary data received");
            return;
        }
        try {
            const buffer = new Buffer.from(message);
            console.log("New Message: ", buffer.toString());

            console.log("Received:", JSON.parse(message));
            let data = JSON.parse(message);
            let motorState = data?.soilMoisture < 50 && data?.waterLevel > 300;

            ws.send(motorState ? "M1" : "M0");

            // Example: Send a response back
            // ws.send("Message received: " + message);

            // Check weather every 5 minutes
            const currentTime = new Date();
            if (
                currentTime - lastWeatherCheck > 60 * 1000 &&
                WEATHER_API_KEY &&
                clientIp
            ) {
                lastWeatherCheck = currentTime;
                axios
                    .get(
                        `${BASE_URL}/forecast.json?key=${WEATHER_API_KEY}&q=${clientIp}&days=1&aqi=no&alerts=no`
                    )
                    .then((response) => {
                        // console.log(response.data);
                        const weather =
                            response.data?.forecast?.forecastday[0]?.hour;
                        // console.log("Weather: ", weather);

                        const nextHour = weather?.find(
                            (hour) => new Date(hour.time) > new Date()
                        );
                        const condition = nextHour?.condition?.text
                            ?.toLowerCase()
                            .trim();
                        console.log("Next Hour: ", condition);

                        // turn off motor if weather is rainy
                        if (condition.regex(/rain/i)) {
                            // Send data to React
                            wss2.clients.forEach((client) => {
                                if (client.readyState === WebSocket.OPEN) {
                                    client.send({
                                        temperature: data.temperature,
                                        humidity: data.humidity,
                                        soilMoisture: data.soilMoisture,
                                        waterLevel: data.waterLevel,
                                        motorState: false,
                                    });
                                }
                            });
                        }
                    })
                    .catch((error) => {
                        console.log(error.message);
                    });
            }

            // Send data to React
            wss2.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    const update = {
                        temperature: data.temperature,
                        humidity: data.humidity,
                        soilMoisture: data.soilMoisture,
                        waterLevel: data.waterLevel,
                        motorState,
                    };
                    client.send(JSON.stringify(update));
                }
            });
        } catch (error) {
            console.log(error);
        }
    });

    ws.on("error", (error) => {
        console.log("Error lasjfklaj: ");

        console.log(error);
    });

    ws.on("close", () => {
        console.log("NodeMCU disconnected");
    });
});

wss2.on("connection", (ws) => {
    console.log("React connected");

    // Receive data from React
    ws.on("message", (message) => {
        const buffer = new Buffer.from(message);
        console.log("React Message: ", buffer.toString());

        // console.log('Received:', JSON.parse(message));

        // Example: Send a response back
        const data = JSON.parse(buffer.toString());
        console.log(data);
        wss.clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                if (data?.motor) {
                    client.send("M1");
                } else {
                    client.send("M0");
                }
            }
        });
    });

    ws.on("error", (error) => {
        console.log("Error lasjfklaj: ");

        console.log(error);
    });

    ws.on("close", () => {
        console.log("React disconnected");
    });
});

// import OpenAI from "openai";

// const openai = new OpenAI({
//   apiKey: "process.env.OPENAI_API_KEY",
// });

// const completion = openai.chat.completions.create({
//   model: "gpt-4o-mini",
//   store: true,
//   messages: [
//     {"role": "user", "content": "write a haiku about ai"},
//   ],
// });

// completion.then((result) => console.log(result.choices[0].message));
