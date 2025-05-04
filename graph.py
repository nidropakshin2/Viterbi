import pandas as pd
import matplotlib.pyplot as plt

# Загрузка данных
df = pd.read_csv("BER.csv", header=0)
data = df.to_numpy()

plt.figure(figsize=(10, 6))

p = data[:, 0]
BER_min = data[:, 1]
BER_avg = data[:, 2]
BER_max = data[:, 3]

plt.plot(p, BER_avg, "b-", label="Среднее BER", linewidth=2)
plt.fill_between(
    p, 
    BER_min, 
    BER_max, 
    color="blue", 
    alpha=0.2, 
    label="Min-Max диапазон"
)

plt.xlabel("Вероятность ошибки (p)", fontsize=12)
plt.ylabel("BER", fontsize=12)
plt.title("Зависимость BER от вероятности ошибки", fontsize=14)
plt.yscale("log")  # Логарифмическая шкала для оси Y
plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.legend()

plt.tight_layout()
plt.savefig("BER.png", dpi=300)
plt.show()