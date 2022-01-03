from flask import Flask
from flask import request, render_template
import time
import datetime
from datetime import datetime
from dateutil import tz
import matplotlib.pyplot as plt
import glob

plt.style.use('dark_background')
FROM_ZONE = tz.gettz('UTC')         # time zone in which time.time() is (unix timestamp)
TO_ZONE = tz.gettz('Europe/Zegreb') # current time zone

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0


OUTPUT_FILE = 'data.txt'
KEY_WIDTH = 10
IMGS_PATH = 'static/'
humidity_threshold = 1000
units = {'brightness':'', 'pressure':'kPa', 'humidity':'', 'temperature':'°C'}
data = [] 

@app.route("/")
def main(tfrom=float('-inf'), tto=float('inf')):
    make_imgs(data, tfrom=tfrom, tto=tto)
    return render_template(
        'index.html', 
        imgs=glob.glob('static/*.png'), 
        current_data=data[-1][0], 
        units=units
    )

@app.route('/get_specific', methods=['POST'])
def get_specific():
    tfrom, tto = float('-inf'), float('inf')
    if 'last year' in request.form:
        tfrom = time.time() - 365*24*60*60
    elif 'last month' in request.form:
        tfrom = time.time() - 30*24*60*60
    elif 'last week' in request.form:
        tfrom = time.time() - 7*24*60*60
    elif 'last day' in request.form:
        tfrom = time.time() - 24*60*60
    elif 'last 8 hours' in request.form:
        tfrom = time.time() - 8*60*60
    elif 'last hour' in request.form:
        tfrom = time.time() - 60*60
    else:
        tfrom = str2unixtime(request.form['from'], float('-inf'))
        tto = str2unixtime(request.form['to'], float('inf'))
    return main(tfrom, tto)

def str2unixtime(s, replace):
    if s!='':
        date_time_obj = datetime.strptime(s, '%d/%m/%y %H:%M:%S').replace(tzinfo=TO_ZONE).astimezone(FROM_ZONE)
        return datetime.timestamp(date_time_obj)
    return replace

@app.route("/get_settings")
def get():
    return {'humidity_threshold':humidity_threshold}

@app.route("/post_data", methods=['POST'])
def post():
    response_dict = request.form
    state = {k.strip():float(response_dict[k]) for k in response_dict}
    data.append((state, time.time()))
    with open(OUTPUT_FILE, 'a') as fout:
        s = f"{time.time():5.4f} -> "
        for k in state:
            s += k + ":" + f"{state[k]:5.4f} "
        fout.write(s + '\n')
    return ""

@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also to cache the rendered page for 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response

def data2xy(data, key, tfrom=float('-inf'), tto=float('inf')):
    '''
    data is a list of tuples (time, state(dict))
    tfrom and tto are unix timestamps
    '''
    x, y = [], []
    for state, t in data:
        if tfrom <= t <= tto:
            y.append(state[key])
            x.append(datetime.utcfromtimestamp(t).replace(tzinfo=FROM_ZONE).astimezone(TO_ZONE))
    return (x, y) if len(x)>0 else ([0, 0], [0, 0])

def make_imgs(data, tfrom=float('-inf'), tto=float('inf')):
    for k in data[0][0]:
        x, y = data2xy(data, k, tfrom, tto)
        plt.plot(x, y)
        plt.xlabel('time')
        plt.xticks(rotation = -20) # Rotates X-Axis Ticks by 45-degrees
        plt.ylabel(k)
        plt.title(k)
        plt.savefig(IMGS_PATH + k + '.png')
        plt.clf()

def load_data():
    '''
    data is written in following format
    utc time -> key:value key:value ...
    '''
    data = []
    with open(OUTPUT_FILE, 'r') as infile:
        for line in infile:
            t, dic = line.split(' -> ')
            tmp_dict = {}
            for kv in dic.split(' ')[:-1]:
                k, v = kv.split(':')
                tmp_dict[k] = float(v)

            data.append((tmp_dict, float(t)))
    return data



data = load_data()


