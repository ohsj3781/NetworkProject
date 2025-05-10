import sys
import dash
from dash import dcc
from dash import html
import plotly.graph_objects as go

file_names = sys.argv[1:]

data = {}
for fn in file_names:
    with open(fn, "r") as f:
        lines = f.readlines()
        lines = [list(map(float, x.split())) for x in lines]
        x_list = [x[0] for x in lines]
        y_list = [x[1] for x in lines]
        data[fn] = [x_list, y_list]

app = dash.Dash()  # initialising dash app

def gen_fig():
    objs = []
    for fn in data:
        obj = go.Scatter(x=data[fn][0], y=data[fn][1],
                         line=dict(width=4), name=fn)
        objs.append(obj)
    fig = go.Figure(objs)
    fig.update_layout(xaxis_title='Time',
                      yaxis_title='CWND')
    return fig

app.layout = html.Div(id='parent', children=[
    html.H1(id='H1', children='TCP CWND Basic 2020314916 오승재', style={'textAlign':'center',
                                                                 'marginTop':40, 'marginBottom':40}),
    dcc.Graph(id='line_plot', figure=gen_fig())
])

if __name__ == '__main__':
    app.run(host="0.0.0.0")

