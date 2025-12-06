import cv2
import mediapipe as mp
import paho.mqtt.client as mqtt
import time

# MQTT
MQTT_BROKER = "broker.emqx.io"
MQTT_PORT = 1883

TOPIC_LIGHT_ON = "home/room1/light/on"
TOPIC_LIGHT_OFF = "home/room1/light/off"
TOPIC_FAN_ON = "home/room1/fan/on"
TOPIC_FAN_OFF = "home/room1/fan/off"
TOPIC_RESET = "home/room1/system/reset"

client = mqtt.Client()
client.connect(MQTT_BROKER, MQTT_PORT, 60)

# MEDIAPIPE Setup
mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils
hands = mp_hands.Hands(max_num_hands=1)


last_action_time = 0
cooldown = 2


def count_fingers(hand_landmarks):
    tips = [8, 12, 16, 20]  # finger tips
    fingers = []

    if hand_landmarks.landmark[4].x < hand_landmarks.landmark[3].x:
        fingers.append(1)
    else:
        fingers.append(0)

    for tip in tips:
        if hand_landmarks.landmark[tip].y < hand_landmarks.landmark[tip - 2].y:
            fingers.append(1)
        else:
            fingers.append(0)

    return fingers.count(1)


# Camera with openCV
cap = cv2.VideoCapture(0)

while True:
    success, frame = cap.read()
    if not success:
        continue

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb)

    if results.multi_hand_landmarks:
        for handLms in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(frame, handLms, mp_hands.HAND_CONNECTIONS)

            finger_count = count_fingers(handLms)

            if time.time() - last_action_time > cooldown:

                # Light controlling
                if finger_count == 5:  # Open hand used for light On(forced)
                    client.publish(TOPIC_LIGHT_ON, "ON")
                    print("Light ON (Gesture: Open Hand)")
                    last_action_time = time.time()

                elif finger_count == 0:  # Fist used for light off(forced)
                    client.publish(TOPIC_LIGHT_OFF, "OFF")
                    print("Light OFF (Gesture: Fist)")
                    last_action_time = time.time()

                # Fan controlling
                elif finger_count == 2:  # Two fingers used for Fan on
                    client.publish(TOPIC_FAN_ON, "ON")
                    print("Fan ON (Gesture: Two Fingers)")
                    last_action_time = time.time()

                elif finger_count == 3:  # Three fingers used for fan off
                    client.publish(TOPIC_FAN_OFF, "OFF")
                    print("Fan OFF (Gesture: Three Fingers)")
                    last_action_time = time.time()

                # Reset system
                elif finger_count == 1:  # One finger used for reset system
                    print("System Reset Gesture Detected")
                    # Example actions on reset:
                    client.publish(TOPIC_RESET, "RESET")
                    print("Reset the Sysem")
                    last_action_time = time.time()

    cv2.imshow("Gesture Control", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
