import matplotlib.pyplot as plt
import numpy as np
import heapq

# --- CONFIGURATION ---
rows, cols = 7, 7
field = np.zeros((rows, cols))
visit_count = np.zeros((rows, cols)) # Tracks how many times each cell is hit
speed_delay = 0.3 
total_moves = 0
overlaps = 0

# Obstacles
obstacles = [(3,3), (3,4), (4,3), (2,3), (1,1), (1,3)]
for x, y in obstacles:
    field[x][y] = -1

plt.ion()
fig, ax = plt.subplots(figsize=(7, 7))

def draw(rx, ry, steps, overlap_num):
    ax.clear()
    # Use 'YlGn' - darker green means more visits (overlap)
    ax.imshow(visit_count, cmap='YlGn', vmin=0, vmax=2)
    
    # Draw obstacles in black
    for ox, oy in obstacles:
        ax.add_patch(plt.Rectangle((oy-0.5, ox-0.5), 1, 1, color='black'))

    ax.set_xticks(range(cols))
    ax.set_yticks(range(rows))
    ax.grid(True, linestyle=':', alpha=0.5)
    
    # Robot
    ax.scatter(ry, rx, c='red', s=200, edgecolors='white', zorder=5)
    
    ax.set_title(f"Steps: {steps} | Overlaps: {overlap_num}\n(Darker Green = Re-visited)", fontsize=10)
    plt.pause(speed_delay)

def get_neighbors(x, y):
    for dx, dy in [(0, 1), (1, 0), (0, -1), (-1, 0)]:
        nx, ny = x + dx, y + dy
        if 0 <= nx < rows and 0 <= ny < cols and field[nx][ny] != -1:
            yield nx, ny

# --- START ---
rx, ry = 0, 0
field[rx][ry] = 1
visit_count[rx][ry] = 1
total_moves += 1
draw(rx, ry, total_moves, overlaps)

while True:
    pq = [(0, rx, ry)]
    parent = {}
    cost_so_far = {(rx, ry): 0}
    found = None

    while pq:
        current_cost, cx, cy = heapq.heappop(pq)
        if field[cx][cy] == 0:
            found = (cx, cy)
            break
        for nx, ny in get_neighbors(cx, cy):
            # Penalty for re-visiting
            step_cost = 1 if field[nx][ny] == 0 else 20 
            new_cost = current_cost + step_cost
            if (nx, ny) not in cost_so_far or new_cost < cost_so_far[(nx, ny)]:
                cost_so_far[(nx, ny)] = new_cost
                heapq.heappush(pq, (new_cost, nx, ny))
                parent[(nx, ny)] = (cx, cy)

    if not found:
        break

    path = []
    curr = found
    while curr != (rx, ry):
        path.append(curr)
        curr = parent[curr]
    path.reverse()

    for px, py in path:
        rx, ry = px, py
        if field[rx][ry] == 1:
            overlaps += 1
        field[rx][ry] = 1
        visit_count[rx][ry] += 1
        total_moves += 1
        draw(rx, ry, total_moves, overlaps)

plt.ioff()
print(f"Final Report: {total_moves} steps, {overlaps} overlaps.")
plt.show()