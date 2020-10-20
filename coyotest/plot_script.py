import pandas as pd
import plotly.express as px

df = pd.read_csv('/home/udit/memcached_2020/memcached/coyotest/memcached_coverage.txt')

fig = px.line(df, x = 'x',  y = 'y', labels =
        {
            'x' : 'Number of iterations',
            'y' : 'Number of Unique states of KV store'
        }, title='Coverage for Memcached.')
fig.show()
