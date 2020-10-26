import pandas as pd
import numpy as np

df = pd.read_csv('advertising.csv')
df.drop('Sales', axis=1, inplace = True)
f = open('data.txt','w+')
data = ""+str(int(df.iloc[0][0]*10))+" "+str(int(df.iloc[0][1]*10))+" "+str(int(df.iloc[0][2]*10))
f.write(data)
f.close()

