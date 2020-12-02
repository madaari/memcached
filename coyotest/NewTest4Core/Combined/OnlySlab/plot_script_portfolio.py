import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
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
    name='With Nekara, Portfolio strategy'
)


fig = make_subplots(1,1);
fig.update_layout(title_text="Memcached coverage. Hash of only Slab using the combined test case");

fig.add_trace(trace1);
fig.add_trace(trace2);

fig.show()
