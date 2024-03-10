#!/usr/bin/env python
from matplotlib.ticker import NullFormatter  # useful for `logit` scale  # noqa
import matplotlib.pyplot as plt  # noqa
import numpy as np
from scipy import signal
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import PySimpleGUI as sg
import matplotlib
import sounddevice as sd
import os
import json


matplotlib.use('TkAgg')

"""
Demonstrates one way of embedding Matplotlib figures into a PySimpleGUI window.

Basic steps are:
 * Create a Canvas Element
 * Layout form
 * Display form (NON BLOCKING)
 * Draw plots onto convas
 * Display form (BLOCKING)

 Based on information from:
 https://matplotlib.org/3.1.0/gallery/user_interfaces/embedding_in_tk_sgskip.html
 (Thank you Em-Bo & dirck)
"""


class SignalModule():
    def __init__(self):
        self.fn = 'sin'
        self.amp = 1.0
        self.freq = 0.0
        self.arg = 0.5
        self.At = 0.0
        self.Dt = 0.1
        self.Slv = 0.3
        self.Rt = 0.2

    def set(self, fn, amp, freq, arg, At, Dt, Slv, Rt):
        self.fn = fn
        self.amp = amp
        self.freq = freq
        self.arg = arg
        self.At = At
        self.Dt = Dt
        self.Slv = Slv
        self.Rt = Rt

    def signal(self, t, t1=0):
        if self.fn == 'sin':
            y = self.amp * np.sin(2 * np.pi * t * self.freq + t1)
        elif self.fn == 'square':
            y = self.amp * signal.square(2 * np.pi * t * self.freq + t1, duty=self.arg)
        else:  # self.fn == 'saw':
            y = self.amp * signal.sawtooth(2 * np.pi * t * self.freq + t1, width=self.arg)
        return y

    def envelope(self, t):
        # fr = self.freq
        fr = f_s  # not N
        end = 1.0
        Rt = end + self.Rt
        Slv = self.Slv
        St = end  # [TODO] fix self.Rt
        env_ts = [self.At * fr, self.Dt * fr, St * fr, Rt * fr]
        env_y = np.array(range(len(t)), dtype=np.float64)
        for i in range(len(env_y)):
            # env_y[i] = y[i]
            if i < env_ts[0]:
                k = (i - 0.0) / (env_ts[0] - 0.0)
            elif i < env_ts[1]:
                k = -(i - env_ts[0]) / (env_ts[1] - env_ts[0]) * (1.0 - Slv) + 1.0
            elif i < env_ts[2]:
                k = Slv
            elif i < env_ts[3]:
                k = -(i - env_ts[2]) / (env_ts[3] - env_ts[2]) * (Slv) + Slv
            else:
                k = 0.0
            # env_y[i] = k * y[i]
            env_y[i] = k
        return env_y


class SignalModuleGadget(SignalModule):
    def __init__(self, name):
        super().__init__()
        self.name = name

    def create(self):
        name = self.name
        gui_fn = [
            [
                sg.Radio('sin', key=f'{name}_sin', group_id=f'{name}_radio', default=True),
                sg.Radio('sq ', key=f'{name}_sqr', group_id=f'{name}_radio'),
                sg.Radio('saw', key=f'{name}_saw', group_id=f'{name}_radio'),
            ]
        ]
        gui_sig = [
            self.create_sig_slider('amp', (0.0, 10.0), 1.0, 0.1),
            self.create_sig_slider('freq', (0, 44100 / 2), 0, 1),
            self.create_sig_slider('arg', (0.1, 1.0), 0.5, 0.1),
        ]
        gui_env = [
            [
                sg.Column([
                    self.create_env_slider_h('A', (0.0, 1.0), 0.0, 0.01),
                    self.create_env_slider_h('D', (0.0, 1.0), 0.1, 0.1),
                    self.create_env_slider_h('S', (0.0, 1.0), 0.2, 0.1),
                    self.create_env_slider_h('R', (0.0, 1.0), 0.2, 0.1),
                ]),
                self.create_env_graph(),
            ]
        ]
        frame_layout = gui_fn + gui_sig + gui_env
        frame = sg.Frame(name, frame_layout, vertical_alignment='top')
        return frame

    def set_values(self, values):
        name = self.name
        if values[f'{name}_sin']:
            fn = 'sin'
        elif values[f'{name}_sqr']:
            fn = 'square'
        else:
            fn = 'saw'
        self.set(
            fn,
            float(values[f'{name}_sig_amp_slider']),
            float(values[f'{name}_sig_freq_slider']),
            float(values[f'{name}_sig_arg_slider']),
            float(values[f'{name}_env_A_slider']),
            float(values[f'{name}_env_D_slider']),
            float(values[f'{name}_env_S_slider']),
            float(values[f'{name}_env_R_slider']),
        )

    def get_json(self):
        return {
            'fn': self.fn,
            'amp': self.amp,
            'freq': self.freq,
            'arg': self.arg,
            'At': self.At,
            'Dt': self.Dt,
            'Slv': self.Slv,
            'Rt': self.Rt,
        }

    def set_json(self, window, jvalues):
        name = self.name
        self.set(*list(jvalues.values()))
        window[f'{name}_sig_amp_slider'].Update(jvalues['amp'])
        window[f'{name}_sig_freq_slider'].Update(jvalues['freq'])
        window[f'{name}_sig_arg_slider'].Update(jvalues['arg'])
        window[f'{name}_env_A_slider'].Update(jvalues['At'])
        window[f'{name}_env_D_slider'].Update(jvalues['Dt'])
        window[f'{name}_env_S_slider'].Update(jvalues['Slv'])
        window[f'{name}_env_R_slider'].Update(jvalues['Rt'])

    def handle_event(self, event, values, window):
        name = self.name
        if event is None:
            return
        elif event == f'{name}_sig_amp_btn':
            window[f'{name}_sig_amp_slider'].Update(values[f'{name}_sig_amp_txt'])
        elif event == f'{name}_sig_freq_btn':
            window[f'{name}_sig_freq_slider'].Update(values[f'{name}_sig_freq_txt'])
        elif event == f'{name}_sig_arg_btn':
            window[f'{name}_sig_arg_slider'].Update(values[f'{name}_sig_arg_txt'])

    def update_graph(self, t, y):
        self.env_subplot.cla()
        self.env_subplot.plot(t, y)
        self.env_fig_canvas_agg.draw()

    def create_sig_slider(self, txt, vrange, default, resolution):
        name = self.name
        col_list = [
            sg.Text(txt),
            sg.Slider(
                vrange, default_value=default, resolution=resolution,
                orientation='h', size=(10, 10),
                enable_events=True, key=f'{name}_sig_{txt}_slider'),
            sg.InputText(default_text=str(default), size=(5, 1), key=f'{name}_sig_{txt}_txt'),
            sg.Button('Set', key=f'{name}_sig_{txt}_btn')
        ]
        return col_list

    def create_env_slider_h(self, txt, vrange, default, resolution):
        name = self.name
        row_list = [
            sg.Text(txt),
            sg.Slider(
                vrange, default_value=default, resolution=resolution,
                orientation='h', size=(10, 10),
                enable_events=True, key=f'{name}_env_{txt}_slider'),
        ]
        return row_list

    def create_env_slider_v(self, txt, vrange, default, resolution):
        name = self.name
        row_list = sg.Column([
            [sg.Text(txt)],
            [sg.Slider(
                vrange, default_value=default, resolution=resolution,
                orientation='v', enable_events=True, key=f'{name}_env_{txt}_slider')],
        ], vertical_alignment='top')
        return row_list

    def create_env_graph(self):
        name = self.name
        self.env_fig = matplotlib.figure.Figure(figsize=(2, 2), dpi=60)
        self.env_subplot = self.env_fig.add_subplot(111)
        env_canvas = sg.Canvas(key=f'{name}_env_canvas')
        return env_canvas

    def setup_env_graph(self, window):
        name = self.name
        self.env_fig_canvas_agg = draw_figure(window[f'{name}_env_canvas'].TKCanvas, self.env_fig)


def algo_graph_update(algo):
    algo_image_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), f'algo{algo}.png')
    algo_graph.draw_image(filename=algo_image_path, location=(0, 120))
    algo_graph.update()


# ------------------------------- PASTE YOUR MATPLOTLIB CODE HERE -------------------------------
#
# # Goal is to have your plot contained in the variable  "fig"
#
# # Fixing random state for reproducibility
# np.random.seed(19680801)

f_s = 44100  # サンプリングレート f_s[Hz] (任意)
t_fin = 2.0  # 収録終了時刻 [s] (任意)
dt = 1 / f_s  # サンプリング周期 dt[s]
N = int(f_s * t_fin)  # サンプル数 [個]

fft_fig = matplotlib.figure.Figure(figsize=(8, 3), dpi=80)
fft_subplot = fft_fig.add_subplot(111)
wave_fig = matplotlib.figure.Figure(figsize=(8, 3), dpi=80)
wave_subplot = wave_fig.add_subplot(111)
# env1_fig = matplotlib.figure.Figure(figsize=(8, 2), dpi=80)
# env1_subplot = env1_fig.add_subplot(111)
# env2_fig = matplotlib.figure.Figure(figsize=(8, 2), dpi=80)
# env2_subplot = env2_fig.add_subplot(111)
t = np.arange(0, t_fin, dt)

def wave(a, b, fc, fm):
    b_sin_fm = b * np.sin(2 * np.pi * fm * t)
    y = a * np.sin(2 * np.pi * fc * t + b_sin_fm)
    y_fft = np.fft.fft(y)
    freq = np.fft.fftfreq(N, d=dt)  # 周波数を割り当てる（※後述）
    Amp = abs(y_fft / (N / 2))  # 音の大きさ（振幅の大きさ）
    return (freq[1:int(N / 2)], Amp[1:int(N / 2)])

# ------------------------------- END OF YOUR MATPLOTLIB CODE -------------------------------

# ------------------------------- Beginning of Matplotlib helper code -----------------------

def draw_figure(canvas, figure):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg


# ------------------------------- Beginning of GUI CODE -------------------------------
sigmod1_gadget = SignalModuleGadget('signal0')
sigmod2_gadget = SignalModuleGadget('signal1')
sigmod3_gadget = SignalModuleGadget('signal2')
sigmod4_gadget = SignalModuleGadget('signal3')
sigmod1_frame = sigmod1_gadget.create()
sigmod2_frame = sigmod2_gadget.create()
sigmod3_frame = sigmod3_gadget.create()
sigmod4_frame = sigmod4_gadget.create()

algo_radios = [
    sg.Text('algorithm'),
    sg.Radio('0', key='algo0', group_id='algo_radio', enable_events=True, default=True),
    sg.Radio('1', key='algo1', group_id='algo_radio', enable_events=True),
    sg.Radio('2', key='algo2', group_id='algo_radio', enable_events=True),
    sg.Radio('3', key='algo3', group_id='algo_radio', enable_events=True),
    sg.Radio('4', key='algo4', group_id='algo_radio', enable_events=True),
    sg.Radio('5', key='algo5', group_id='algo_radio', enable_events=True),
    sg.Radio('6', key='algo6', group_id='algo_radio', enable_events=True),
    sg.Radio('7', key='algo7', group_id='algo_radio', enable_events=True),
]

manip_layout = sg.Column([
    algo_radios,
    [sg.Graph(
        key='algo_graph',
        canvas_size=(140, 120), graph_bottom_left=(0, 0), graph_top_right=(140, 120),
        background_color='white',
    )],
    [sigmod1_frame, sigmod2_frame],
    [sigmod3_frame, sigmod4_frame],
    # [sg.Button('Play', key='play')],
    [
        sg.Button('Play', key='play'),
        # sg.Button('Save', key='save'),
        sg.FileSaveAs('Save As', key='saveas', target='saveas', enable_events=True),
        sg.FileBrowse('Load', key='load', target='load', enable_events=True),
    ]],
    vertical_alignment='top')
fft_canvas = sg.Canvas(key='fft_canvas')
wave_canvas = sg.Canvas(key='wave_canvas')
wave_layout = [
    [fft_canvas],
    [wave_canvas],
]
layout = [
    [
        sg.Frame('wave', wave_layout, vertical_alignment='top'),
        manip_layout,
    ],
]
# fft_fig_canvas_agg = draw_figure(fft_canvas.TKCanvas, fft_fig)
# wave_fig_canvas_agg = draw_figure(wave_canvas.TKCanvas, wave_fig)

# create the form and show it without the plot
window = sg.Window(
    'Demo Application - Embedding Matplotlib In PySimpleGUI',
    layout, finalize=True, element_justification='center', font='Helvetica 12')

# add the plot to the window
fft_fig_canvas_agg = draw_figure(window['fft_canvas'].TKCanvas, fft_fig)
wave_fig_canvas_agg = draw_figure(window['wave_canvas'].TKCanvas, wave_fig)
sigmod1_gadget.setup_env_graph(window)
sigmod2_gadget.setup_env_graph(window)
sigmod3_gadget.setup_env_graph(window)
sigmod4_gadget.setup_env_graph(window)

algo = 0
algo_image_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), f'algo{algo}.png')
algo_graph = window['algo_graph']
algo_graph.draw_image(filename=algo_image_path, location=(0, 120))
algo_graph.update()

json_path = None

while True:
    event, values = window.read()
    if event is None:
        break
    # algo = 0
    if any([event == f'algo{i}' for i in range(8)]):
        for i in range(8):
            if values[f'algo{i}']:
                algo = i
                break
    sigmod1_gadget.handle_event(event, values, window)
    sigmod2_gadget.handle_event(event, values, window)
    sigmod3_gadget.handle_event(event, values, window)
    sigmod4_gadget.handle_event(event, values, window)

    sigmod1_gadget.set_values(values)
    sigmod2_gadget.set_values(values)
    sigmod3_gadget.set_values(values)
    sigmod4_gadget.set_values(values)

    if algo == 0:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t, y4)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t, y3)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t, y2)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y1
    elif algo == 1:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t, y4 + y3)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t, y2)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y1
    elif algo == 2:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t, y3)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t, y4 + y2)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y1
    elif algo == 3:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t, y4)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t, y3 + y2)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y1
    elif algo == 4:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t, y4)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t, y2)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y3 + y1
    elif algo == 5:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t, y4)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t, y4)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t, y4)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y3 + y2 + y1
    elif algo == 6:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t, y4)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y3 + y2 + y1
    elif algo == 7:
        y4 = sigmod4_gadget.signal(t)
        k4 = sigmod4_gadget.envelope(t)
        y4 = k4 * y4
        y3 = sigmod3_gadget.signal(t)
        k3 = sigmod3_gadget.envelope(t)
        y3 = k3 * y3
        y2 = sigmod2_gadget.signal(t)
        k2 = sigmod2_gadget.envelope(t)
        y2 = k2 * y2
        y1 = sigmod1_gadget.signal(t)
        k1 = sigmod1_gadget.envelope(t)
        y1 = k1 * y1
        y = y4 + y3 + y2 + y1

    y_fft = np.fft.fft(y)
    freq = np.fft.fftfreq(N, d=dt)  # 周波数を割り当てる（※後述）
    Amp = abs(y_fft / (N / 2))  # 音の大きさ（振幅の大きさ）

    fft_subplot.cla()
    # subplot.plot(t, y)
    fft_subplot.plot(freq[1:int(N / 2)], Amp[1:int(N / 2)])
    fft_fig_canvas_agg.draw()
    wave_subplot.cla()
    wave_subplot.plot(t[1:int(f_s / 10)], y[1:int(f_s / 10)])
    wave_fig_canvas_agg.draw()
    sigmod1_gadget.update_graph(t, k1)
    sigmod2_gadget.update_graph(t, k2)
    sigmod3_gadget.update_graph(t, k3)
    sigmod4_gadget.update_graph(t, k4)
    env_y = k1 * y
    # env_y = np.copy(y)
    # for i in range(len(y)):
    #     env_y[i] = k1[i] * y[i]

    algo_graph_update(algo)

    if event is None:
        break
    elif event == 'fc_btn':
        window['fc_slider'].Update(values['fc_txt'])
    elif event == 'fm_btn':
        window['fm_slider'].Update(values['fm_txt'])
    elif event == 'play':
        sd.play(env_y, N)
    elif event == 'saveas':
        if not json_path:
            break
        sigs_json = {
            'algo': algo,
            'signals': [
                sigmod1_gadget.get_json(),
                sigmod2_gadget.get_json(),
                sigmod3_gadget.get_json(),
                sigmod4_gadget.get_json(),
            ],
        }
        json_path = values['saveas']
        with open(json_path, 'w') as f:
            f.write(json.dumps(sigs_json))
    elif event == 'save':
        sigs_json = {
            'algo': algo,
            'signals': [
                sigmod1_gadget.get_json(),
                sigmod2_gadget.get_json(),
                sigmod3_gadget.get_json(),
                sigmod4_gadget.get_json(),
            ],
        }
        with open(json_path, 'w') as f:
            f.write(json.dumps(sigs_json))
    elif event == 'load':
        try:
            sigs_json = None
            with open(values['load'], 'r') as f:
                sigs_json = json.load(f)
            json_path = values['load']
            algo = sigs_json['algo']
            for i in range(8):
                window[f'algo{i}'].Update(False)
                if i == algo:
                    window[f'algo{i}'].Update(True)
            sigmod1_gadget.set_json(window, sigs_json['signals'][0])
            sigmod2_gadget.set_json(window, sigs_json['signals'][1])
            sigmod3_gadget.set_json(window, sigs_json['signals'][2])
            sigmod4_gadget.set_json(window, sigs_json['signals'][3])
            algo_graph_update(algo)
        except Exception as e:
            print(e)
            pass


window.close()
