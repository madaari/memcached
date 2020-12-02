import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from plotly.graph_objs import *
from plotly.subplots import make_subplots

df1 = pd.read_csv('./WithoutNekara/memcached_coverage.txt')

trace1 = go.Scatter(
    x=df1['x'],
    y=df1['y'],
    name='Without Nekara'
)

df2 = pd.read_csv('./WithNekara/memcached_coverage.txt')

trace2 = go.Scatter(
    x=df2['x'],
    y=df2['y'],
    name='With Nekara'
)


fig = make_subplots(1,1);
fig.update_layout(title_text="Memcached", title_x=0.5, title_y=0.87, paper_bgcolor='rgba(0,0,0,0)', plot_bgcolor='rgba(0,0,0,0)',legend=dict(
    yanchor="top",
    y=0.995,
    xanchor="left",
    x=0.01
),
font=dict(
        family="Ariel",
        size=26,
        color="Black"
));

fig.update_xaxes(showgrid=True, gridwidth=1, gridcolor='lightgrey', zeroline=True, zerolinewidth=2, zerolinecolor='Black', title_text="# iterations")
fig.update_yaxes(showgrid=True, gridwidth=1, gridcolor='lightgrey', zeroline=True, zerolinewidth=2, zerolinecolor='Black', title_text="# unique abstract state hashes")

fig.add_trace(trace1);
fig.add_trace(trace2);

fig.show()
