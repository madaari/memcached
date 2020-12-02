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

df2 = pd.read_csv('./WithNekara/memcached_coverage_random.txt')

trace2 = go.Scatter(
    x=df2['x'],
    y=df2['y'],
    name='With Nekara, Random'
)

df3 = pd.read_csv('./WithNekara/memcached_coverage_random_probabilistic_extreme_300_steps.txt')

trace3 = go.Scatter(
    x=df3['x'],
    y=df3['y'],
    name='With Nekara, Random Probabilistic, 300 steps, with extreme probabilities'
)

df4 = pd.read_csv('./WithNekara/memcached_coverage_random_probabilistic_extreme_2000_steps.txt')

trace4 = go.Scatter(
    x=df4['x'],
    y=df4['y'],
    name='With Nekara, Random Probabilistic, 2000 steps, with extreme probability'
)

fig = make_subplots(1,1);
fig.update_layout(title_text="Memcached coverage. Hash of only LRU only with just 4 set and 8 gets");

fig.add_trace(trace1);
fig.add_trace(trace2);
fig.add_trace(trace3);
fig.add_trace(trace4);

fig.show()
