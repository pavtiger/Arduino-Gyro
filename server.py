from dataclasses import dataclass
from threading import Thread
import unicodedata as ud
import json
import math
import time
from random import randint

import eventlet
from flask_socketio import SocketIO
from flask import Flask, send_from_directory, render_template

from ip import ip_address, port

async_mode = None
app = Flask(__name__, static_url_path='')
socketio = SocketIO(app, async_mode=async_mode)

latin_letters = {}
SPAWN_R = 400



@dataclass
class Point3d:
    x: float
    y: float
    z: float


# Vector structure for collisions
class Point:
    def __init__(self, a, b):
        if type(a) == Point and type(b) == Point:
            self.x = b.x - a.x
            self.y = b.y - a.y
        else:
            self.x = a
            self.y = b

    def dp(self, other):
        return self.x * other.x + other.y * self.y

    def cp(self, other):
        return self.x * other.y - other.x * self.y



# Main page
@app.route('/')
def root():
    return render_template('main.html')


# Get files from server (etc. libs)
@app.route('/js/<path:path>')
def send_js(path):
    return send_from_directory('js', path)


# Used for checking a name entered by the user
@app.route('/check/<username>')
def check(username):
    if not only_roman_chars(username):
        return '{"status": "", "error": "true"}'

    return ['{"status": "false", "error": "false"}', '{"status": "true", "error": "false"}'][
        username in TheGrid.AllPlayers.keys()]


@socketio.on('keyup')
def up(data):
    username = data['user']
    try:
        if data['key'] == 65:  # A
            TheGrid.AllPlayers[username].max_turn_angle = -0.0001
        elif data['key'] == 68: # D
            TheGrid.AllPlayers[username].max_turn_angle = 0.0001
    except:
        pass


@socketio.on('keydown')
def down(data):
    username = data['user']
    try:
        if data['key'] == 65:  # A
            TheGrid.AllPlayers[username].max_turn_angle = 0.7
        elif data['key'] == 68:  # D
            TheGrid.AllPlayers[username].max_turn_angle = -0.7
        elif data['key'] == 16:  # Shift
            TheGrid.AllPlayers[username].speed = TheGrid.Speed * 3
            TheGrid.AllPlayers[username].boost_time = 2000 * TheGrid.AllPlayers[username].booster
            TheGrid.AllPlayers[username].booster = 0
        elif data['key'] == 67:  # C
            TheGrid.AllPlayers[username].toggle_controls_rotation = not TheGrid.AllPlayers[
                username].toggle_controls_rotation
    except:
        pass


@socketio.on('message')
def handle_message(data):
    pass


# When user chooses a name he submits his final name and we add him to the table
@socketio.on('add_user')
def add(username, mobile):
    print(mobile)
    if username not in TheGrid.AllPlayers.keys():
        if len(TheGrid.AllPlayers) == 1:
            TheGrid.player_reset()

        TheGrid.LastTrail[username] = Point3d(0, 0, 0)
        angle = TheGrid.StartPositions[TheGrid.UsersNum] * math.pi / 180
        a = Point(SPAWN_R * math.cos(angle), SPAWN_R * math.sin(angle))
        b = Point(0, 800)

        TheGrid.AllPlayers[username] = Player(SPAWN_R * math.cos(angle), 0, SPAWN_R * math.sin(angle),
                                              username, TheGrid.Speed, math.atan2(a.cp(b), a.dp(b)) - math.pi, [], [], [])
        TheGrid.UsersNum += 1
        converted = []
        for booster in TheGrid.boosters:
            converted.append({"x": booster.x, "y": booster.y, "z": booster.z })
        socketio.emit('booster', json.dumps(converted))


@socketio.on('remove_user')
def remove_user(username):
    print('remove_user')
    try:
        del TheGrid.AllPlayers[username]
    except:
        pass


# def game_loop(name):
#     while True:
#         time.sleep(0.01)



if __name__ == "__main__":
    eventlet.monkey_patch()

    # x = Thread(target=game_loop, args=(1,))
    # x.start()

    socketio.run(app, host=ip_address, port=port)
