import signal_processor_new_prob as sp
from pyswip import Prolog

main_signal = sp.Signal([1.0, 2.0, 3.0, 4.0, 5.0])
fir_example = sp.FIR("ExampleFilter", [0.5, 0.5])

prolog = Prolog()
prolog.assertz("depends_on('FIR1', 'SUM1')")  
prolog.assertz("depends_on('IIR2', 'SUM1')")
prolog.assertz("depends_on(filter(X), depends(X, Y)) :- depends_on(X, Y)") 

# найти все зависимости для SUM1
query = list(prolog.query("depends_on(X, 'SUM1')"))
print("Фильтры, от которых зависит SUM1:")
for result in query:
    print(result["X"])

# найти все фильтры, зависящие от FIR1
query = list(prolog.query("depends_on('FIR1', Y)"))
print("\nФильтры, зависящие от FIR1:")
for result in query:
    print(result["Y"])