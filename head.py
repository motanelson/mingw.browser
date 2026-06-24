import os
down="https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html"
files="down.html"
print("\033c\033[47;31m\nfile: "+files)
os.system("curl "+down+" -o "+files)
f1=open(files,"r")
a=f1.read()
f1.close()
b=a.find("<body")
if b<0:
    b=a.find("<BODY")
a=a[b:]
f1=open(files,"w")
f1.write(a)
f1.close()
