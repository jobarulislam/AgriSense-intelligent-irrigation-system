import { useState, useEffect, useRef, useCallback } from "react";
import { Card, Button, Table, Tag, Spin, Badge, Switch } from "antd";
import useWebSocket, { ReadyState } from "react-use-websocket";

const SensorDashboard = ({ state }: { state: string }) => {
  const [sensorData, setSensorData] = useState<{
    temperature: string | null;
    humidity: string | null;
    soilMoisture: string | null;
    waterLevel: string | null;
    motorState: boolean | null;
  }>({
    temperature: null,
    humidity: null,
    soilMoisture: null,
    waterLevel: null,
    motorState: null,
  });
  const [activities, setActivities] = useState<
    {
      key: number;
      action: string;
      status: string;
      timestamp: string;
    }[]
  >([]);
  const [loading, setLoading] = useState(false);
  const [AI, setAI] = useState(false);
  const [motor, setMotor] = useState(false);

  const didUnmount = useRef(false);
  const { sendMessage, lastMessage, readyState } = useWebSocket(
    "ws://20.2.89.216:8001/",
    {
      shouldReconnect: (_closeEvent) => {
        return didUnmount.current === false;
      },
      reconnectAttempts: 10,
      reconnectInterval: 3000,
    },
  );

  useEffect(() => {
    return () => {
      didUnmount.current = true;
    };
  }, []);

  useEffect(() => {
    const data = lastMessage?.data;
    try {
      if (lastMessage && lastMessage !== null && data !== null) {
        if (data === "M0") {
          setSensorData((prev) => ({ ...prev, motorState: false }));
          return;
        }
        // lastMessage.data.then((data: string) => {
        try {
          const message = JSON.parse(data);
          console.log("Received message", message);
          setSensorData({
            temperature: `${message.temperature} *C`,
            humidity: `${message.humidity} %`,
            soilMoisture: `${message.soilMoisture} %`,
            waterLevel: `${message.waterLevel} %`,
            motorState: message.motorState,
          });
        } catch (error) {
          console.error("Error parsing JSON", error);
        }
        // });
      }
    } catch (error) {
      console.error("Error parsing JSON", error);
    }
  }, [lastMessage]);

  const connectionStatus = {
    [ReadyState.CONNECTING]: "Connecting",
    [ReadyState.OPEN]: "Open",
    [ReadyState.CLOSING]: "Closing",
    [ReadyState.CLOSED]: "Closed",
    [ReadyState.UNINSTANTIATED]: "Uninstantiated",
  };

  useEffect(() => {
    if (readyState !== ReadyState.OPEN) {
      console.log("WebSocket not connected");
    }
  }, [readyState]);

  const handleManualTrigger = () => {
    setLoading(true);
    try {
      sendMessage(
        JSON.stringify({
          action: "manual_trigger",
          motor: !sensorData.motorState,
        }),
      );
      setActivities((prev) => [
        {
          key: prev.length + 1,
          action: "Manual Trigger",
          status: sensorData.motorState ? "On" : "Off",
          timestamp: new Date().toLocaleString(),
        },
        ...prev,
      ]);
      // setMotor((prev) => !prev);
      setLoading(false);
    } catch (error) {
      console.error("Error sending POST request", error);
    } finally {
      setLoading(false);
    }
  };

  const columns = [
    {
      title: "Action",
      dataIndex: "action",
      key: "action",
    },
    {
      title: "Status",
      dataIndex: "status",
      key: "status",
      render: (status: string) => (
        <Tag
          color={
            ["Completed", "Enabled", "On"].includes(status) ? "green" : "red"
          }
        >
          {status}
        </Tag>
      ),
    },
    {
      title: "Timestamp",
      dataIndex: "timestamp",
      key: "timestamp",
    },
  ];

  return (
    <div className="p-6 bg-gray-100 min-h-screen w-screen space-y-6">
      {/* Sensor Data */}
      <Card
        title={
          <Badge
            status={
              state === "Open" || state === "Connecting"
                ? "processing"
                : "error"
            }
            text="Real-Time Sensor Data"
          />
        }
        bordered={false}
        className="shadow-lg"
      >
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4 text-center">
          {Object.entries(sensorData).map(([key, value]) => (
            <div key={key} className="p-4 bg-blue-50 rounded-lg shadow">
              <h3 className="text-lg font-semibold text-gray-700 capitalize">
                {key}
              </h3>
              <p className="text-2xl text-gray-800">
                {typeof value === "string" ? (
                  value
                ) : typeof value === "boolean" ? (
                  value ? (
                    "ON"
                  ) : (
                    "OFF"
                  )
                ) : (
                  <Spin />
                )}
              </p>
            </div>
          ))}
        </div>
      </Card>

      {/* Manual Motor Pump Trigger */}
      <Card bordered={false} className="shadow-lg">
        <div className="flex justify-center gap-5">
          <Card title="Manual Trigger" className="shadow-lg">
            <Button
              type="primary"
              size="large"
              loading={loading}
              onClick={handleManualTrigger}
              danger={sensorData.motorState ? true : false}
            >
              {sensorData.motorState ? "Turn Off Motor" : "Turn On Motor"}
            </Button>
          </Card>
          <Card title="AI Mode" className="shadow-lg">
            <Switch
              checked={AI}
              onChange={(checked) => {
                setActivities((prev) => [
                  {
                    key: prev.length + 1,
                    action: "AI Mode",
                    status: checked ? "Enabled" : "Disabled",
                    timestamp: new Date().toLocaleString(),
                  },
                  ...prev,
                ]);
                setAI(checked);
              }}
            />
          </Card>
        </div>
      </Card>

      {/* Activity Table */}
      <Card title="Activity Log" bordered={false} className="shadow-lg">
        <Table
          columns={columns}
          dataSource={activities}
          pagination={{ pageSize: 5 }}
        />
      </Card>
    </div>
  );
};

export default SensorDashboard;
