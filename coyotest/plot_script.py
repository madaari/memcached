import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots

df = pd.read_csv('/home/udit/memcached_2020/memcached/coyotest/BaseLine/OnlySlab/memcached_coverage.txt')

trace2 = go.Scatter(
    x=df['x'],
    y=df['y'],
    name='Without Nekara, Random'
)

df1 = pd.read_csv('/home/udit/memcached_2020/memcached/coyotest/Tests/OnlySlab/memcached_coverage.txt')

trace1 = go.Scatter(
    x=df1['x'],
    y=df1['y'],
    name='With Nekara, Random'
)

fig = make_subplots(1,1);
fig.update_layout(title_text="Memcached coverage. Hash of only Slab");
fig.add_trace(trace1);
fig.add_trace(trace2);

fig.show()
