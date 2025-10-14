#!/bin/bash
# Comprehensive test suite with 100 varied cases and analysis

set -e

# Use DIR variable from environment, default to . if not set
DIR=${DIR:-.}

echo "=========================================="
echo "Comprehensive Metro Map Testing Suite (Deterministic)"
echo "Deterministic parameter sweep over N, M, K, J"
echo "Using directory: $DIR"
echo "=========================================="
echo ""

# Create directories
mkdir -p /tmp/metro_tests
mkdir -p images
mkdir -p test_results

# # Cleanup function
# cleanup() {
#     if [ "$1" != "keep" ]; then
#         echo "Cleaning up temporary files..."
#         rm -rf /tmp/metro_tests/*
#     fi
# }

# Set trap to cleanup on error
# trap 'cleanup; exit 1' ERR

# Compile
echo "[SETUP] Compiling encoder and decoder from $DIR..."
g++ $DIR/run1.cpp -o $DIR/run1
g++ $DIR/run2.cpp -o $DIR/run2
echo "✓ Compilation successful"
echo ""

# Test counter
TOTAL=0
PASSED=0
FAILED=0
UNSAT_COUNT=0

# Arrays to store statistics and parameters
declare -a N_ARRAY
declare -a M_ARRAY
declare -a K_ARRAY
declare -a J_ARRAY
declare -a VARS_ARRAY
declare -a CLAUSES_ARRAY
declare -a LITERALS_ARRAY
declare -a ENCODE_TIMES
declare -a SOLVE_TIMES

# Function to run a single test
run_test() {
    local name=$1
    local N=$2
    local M=$3
    local K=$4
    local J=$5
    local mode=$6
    local seed=$7
    
    TOTAL=$((TOTAL + 1))
    
    # Generate test case in temp directory
    python3 TA/testcase_gen.py --N $N --M $M --K $K --J $J --mode $mode --seed $seed \
        --outdir /tmp/metro_tests --prefix "test_${TOTAL}" --count 1 > /dev/null 2>&1
    local base="/tmp/metro_tests/test_${TOTAL}_000"
    
    # Encode and time encoding
    local enc_start_time=$(date +%s.%N)
    ./$DIR/metro ${base}.city > /dev/null 2>&1
    local enc_end_time=$(date +%s.%N)
    local encode_time=$(echo "$enc_end_time - $enc_start_time" | bc)
    
    # Extract SAT stats and calculate avg literals per clause
    local vars=$(head -1 ${base}.satinput | awk '{print $3}')
    local clauses=$(head -1 ${base}.satinput | awk '{print $4}')
    
    # Calculate actual average literals per clause by counting from the file
    local total_literals=$(tail -n +2 ${base}.satinput | awk '{for(i=1;i<=NF;i++) if($i!=0) count++} END {print count}')
    local avg_lit_per_clause=$(echo "scale=4; $total_literals / $clauses" | bc)
    
    N_ARRAY+=($N)
    M_ARRAY+=($M)
    K_ARRAY+=($K)
    J_ARRAY+=($J)
    VARS_ARRAY+=($vars)
    CLAUSES_ARRAY+=($clauses)
    LITERALS_ARRAY+=($avg_lit_per_clause)
    ENCODE_TIMES+=($encode_time)
    
    # Run MiniSat and capture time
    local start_time=$(date +%s.%N)
    minisat ${base}.satinput ${base}.satoutput > /dev/null 2>&1 || true
    local end_time=$(date +%s.%N)
    local solve_time=$(echo "$end_time - $start_time" | bc)
    SOLVE_TIMES+=($solve_time)
    
    # Decode
    ./$DIR/decoder ${base}.city ${base}.satoutput > /dev/null 2>&1
    
    # Check if UNSAT
    if grep -q "^0$" ${base}.metromap 2>/dev/null; then
        UNSAT_COUNT=$((UNSAT_COUNT + 1))
        echo "[$TOTAL] $name (N=$N,M=$M,K=$K,J=$J): UNSAT (encode=${encode_time}s, sat=${solve_time}s, $vars vars, $clauses clauses, ${avg_lit_per_clause} lit/clause)"
        PASSED=$((PASSED + 1))
        return
    fi
    
    # Validate
    if python3 TA/format_checker.py ${base} > /dev/null 2>&1; then
        echo "[$TOTAL] $name (N=$N,M=$M,K=$K,J=$J): ✓ PASS (encode=${encode_time}s, sat=${solve_time}s, $vars vars, $clauses clauses, ${avg_lit_per_clause} lit/clause)"
        PASSED=$((PASSED + 1))
        
        # Save visualization for selected cases
        # if [ $((TOTAL % 20)) -eq 0 ]; then
        #     python3 $DIR/visualize.py ${base}.city ${base}.metromap images/test_${TOTAL}.png > /dev/null 2>&1
        #     echo "    → Saved visualization to images/test_${TOTAL}.png"
        # fi
    else
        echo "[$TOTAL] $name (N=$N,M=$M,K=$K,J=$J): ✗ FAIL"
        FAILED=$((FAILED + 1))
    fi
}

echo "=========================================="
echo "Generating and Running Deterministic Test Cases"
echo "=========================================="
echo ""

# Deterministic grids for N and M up to 50, plus a big 80x80
Ns=(10 20 30 40 50)
Ms=(10 20 30 40 50)
PCTS=(30 50 70)  # K as % of min(N,M), capped at 70%
Js=(1 2 3)

echo "=== Grid up to 50 (N,M in {10,20,30,40,50}; K in {30%,50%,70%}; J in {1,2,3}) ==="
for N in "${Ns[@]}"; do
    for M in "${Ms[@]}"; do
        minNM=$(( N < M ? N : M ))
        for pct in "${PCTS[@]}"; do
            K=$(( minNM * pct / 100 ))
            if [ $K -lt 1 ]; then K=1; fi
            if [ $K -gt $(( (minNM * 70) / 100 )) ]; then K=$(( (minNM * 70) / 100 )); fi
            if [ $K -lt 1 ]; then K=1; fi
            for J in "${Js[@]}"; do
                seed=$(( N*1000000 + M*10000 + K*100 + J ))
                run_test "Grid-N${N}-M${M}-K${K}-J${J}" $N $M $K $J constructive $seed
            done
        done
    done
done

echo ""
echo "=== Very Big Case (80x80) ==="
N=80; M=80; minNM=80
for pct in 50 70; do
    K=$(( minNM * pct / 100 ))
    if [ $K -lt 1 ]; then K=1; fi
    if [ $K -gt $(( (minNM * 70) / 100 )) ]; then K=$(( (minNM * 70) / 100 )); fi
    for J in "${Js[@]}"; do
        seed=$(( N*1000000 + M*10000 + K*100 + J ))
        run_test "Big-N${N}-M${M}-K${K}-J${J}" $N $M $K $J constructive $seed
    done
done

echo ""
echo "=========================================="
echo "Test Summary"
echo "=========================================="
echo "Total:       $TOTAL"
echo "Passed:      $PASSED ✓"
echo "Failed:      $FAILED ✗"
echo "UNSAT cases: $UNSAT_COUNT"
echo ""

# Generate analysis
echo "=========================================="
echo "Generating Analysis"
echo "=========================================="
echo ""

# Create Python script for analysis
cat > /tmp/analyze_results.py << 'PYEOF'
import sys
import matplotlib.pyplot as plt
import numpy as np

"""
Expected input CSV from stdin:
N,M,K,J,Vars,Clauses,AvgLitsPerClause,EncodeTime,SolveTime
"""

# Read data from stdin
N_list = []
M_list = []
K_list = []
J_list = []
vars_array = []
clauses_array = []
literals_array = []
encode_times = []
solve_times = []

for line in sys.stdin:
    parts = line.strip().split(',')
    if len(parts) == 9:
        N_list.append(int(parts[0]))
        M_list.append(int(parts[1]))
        K_list.append(int(parts[2]))
        J_list.append(int(parts[3]))
        vars_array.append(int(parts[4]))
        clauses_array.append(int(parts[5]))
        literals_array.append(float(parts[6]))
        encode_times.append(float(parts[7]))
        solve_times.append(float(parts[8]))

# Create figure with 6 subplots
fig = plt.figure(figsize=(18, 12))
gs = fig.add_gridspec(3, 2, hspace=0.3, wspace=0.3)
ax1 = fig.add_subplot(gs[0, 0])
ax2 = fig.add_subplot(gs[0, 1])
ax3 = fig.add_subplot(gs[1, 0])
ax4 = fig.add_subplot(gs[1, 1])
ax5 = fig.add_subplot(gs[2, 0])
ax6 = fig.add_subplot(gs[2, 1])

# 1. Histogram of Average Literals per Clause
ax1.hist(literals_array, bins=30, color='steelblue', edgecolor='black', alpha=0.7)
ax1.set_xlabel('Average Literals per Clause', fontsize=12)
ax1.set_ylabel('Frequency', fontsize=12)
ax1.set_title('Distribution of Avg Literals/Clause', fontsize=14, fontweight='bold')
ax1.grid(True, alpha=0.3)
ax1.axvline(np.mean(literals_array), color='red', linestyle='--', linewidth=2, label=f'Mean: {np.mean(literals_array):.2f}')
ax1.legend()

# 2. Variables vs Clauses scatter colored by solve time
scatter = ax2.scatter(vars_array, clauses_array, alpha=0.6, c=solve_times, cmap='viridis', s=20)
ax2.set_xlabel('Variables', fontsize=12)
ax2.set_ylabel('Clauses', fontsize=12)
ax2.set_title('Variables vs Clauses (colored by SAT time)', fontsize=14, fontweight='bold')
ax2.grid(True, alpha=0.3)
cbar = plt.colorbar(scatter, ax=ax2)
cbar.set_label('SAT Solve Time (s)', fontsize=10)

# 3. Time distributions
ax3.hist(encode_times, bins=25, color='slateblue', edgecolor='black', alpha=0.7, label='Encode')
ax3.hist(solve_times, bins=25, color='coral', edgecolor='black', alpha=0.5, label='SAT')
ax3.set_xlabel('Time (seconds)', fontsize=12)
ax3.set_ylabel('Frequency', fontsize=12)
ax3.set_title('Encoding vs SAT Solve Time Distributions', fontsize=14, fontweight='bold')
ax3.legend()
ax3.grid(True, alpha=0.3)

# 4. Complexity growth
indices = list(range(1, len(vars_array) + 1))
ax4.plot(indices, vars_array, 'o-', label='Variables', alpha=0.6, markersize=3)
ax4.plot(indices, clauses_array, 's-', label='Clauses', alpha=0.6, markersize=3)
ax4.set_xlabel('Test Case Number', fontsize=12)
ax4.set_ylabel('Count', fontsize=12)
ax4.set_title('Encoding Complexity Across Test Cases', fontsize=14, fontweight='bold')
ax4.legend(fontsize=10)
ax4.grid(True, alpha=0.3)

# 5. Time vs problem size
problem_sizes = [v*c for v, c in zip(vars_array, clauses_array)]
scatter2 = ax5.scatter(problem_sizes, encode_times, alpha=0.6, c=solve_times, cmap='plasma', s=20, label='Encode')
ax5.set_xlabel('Problem Size (vars × clauses)', fontsize=12)
ax5.set_ylabel('Encode Time (s)', fontsize=12)
ax5.set_title('Encode Time vs Problem Size (color = SAT time)', fontsize=14, fontweight='bold')
ax5.grid(True, alpha=0.3)
cbar2 = plt.colorbar(scatter2, ax=ax5)
cbar2.set_label('SAT Solve Time (s)', fontsize=10)

# 6. Times over test progression
ax6.plot(indices, encode_times, 'o-', color='purple', alpha=0.6, markersize=3, label='Encode')
ax6.plot(indices, solve_times, 'o-', color='orange', alpha=0.6, markersize=3, label='SAT')
ax6.set_xlabel('Test Case Number', fontsize=12)
ax6.set_ylabel('Time (s)', fontsize=12)
ax6.set_title('Encode & SAT Times Across Test Cases', fontsize=14, fontweight='bold')
ax6.legend(fontsize=10)
ax6.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('test_results/comprehensive_analysis.png', dpi=150, bbox_inches='tight')
print("✓ Saved comprehensive analysis to test_results/comprehensive_analysis.png")

# Generate summary statistics
print("\n========================================")
print("Statistical Summary")
print("========================================")
print(f"Variables:       min={min(vars_array)}, max={max(vars_array)}, avg={np.mean(vars_array):.1f}, std={np.std(vars_array):.1f}")
print(f"Clauses:         min={min(clauses_array)}, max={max(clauses_array)}, avg={np.mean(clauses_array):.1f}, std={np.std(clauses_array):.1f}")
print(f"Lits/Clause:     min={min(literals_array):.4f}, max={max(literals_array):.4f}, avg={np.mean(literals_array):.4f}, std={np.std(literals_array):.4f}")
print(f"Encode times:    min={min(encode_times):.4f}s, max={max(encode_times):.4f}s, avg={np.mean(encode_times):.4f}s, std={np.std(encode_times):.4f}s")
print(f"SAT times:       min={min(solve_times):.4f}s, max={max(solve_times):.4f}s, avg={np.mean(solve_times):.4f}s, std={np.std(solve_times):.4f}s")

# Save detailed CSV
with open('test_results/test_statistics.csv', 'w') as f:
    f.write("Test,N,M,K,J,Variables,Clauses,AvgLiteralsPerClause,EncodeTime,SolveTime\n")
    for i, (N, M, K, J, v, c, l, et, st) in enumerate(zip(N_list, M_list, K_list, J_list, vars_array, clauses_array, literals_array, encode_times, solve_times), 1):
        f.write(f"{i},{N},{M},{K},{J},{v},{c},{l},{et},{st}\n")
print(f"\n✓ Saved detailed statistics to test_results/test_statistics.csv")
PYEOF

# Feed data to Python analysis script
for i in "${!VARS_ARRAY[@]}"; do
    echo "${N_ARRAY[$i]},${M_ARRAY[$i]},${K_ARRAY[$i]},${J_ARRAY[$i]},${VARS_ARRAY[$i]},${CLAUSES_ARRAY[$i]},${LITERALS_ARRAY[$i]},${ENCODE_TIMES[$i]},${SOLVE_TIMES[$i]}"
done | python3 /tmp/analyze_results.py

echo ""

# Cleanup temp files
echo "Cleaning up temporary test files..."
rm -rf /tmp/metro_tests
echo "✓ Temporary files cleaned"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "🎉 All tests passed!"
    exit 0
else
    echo "⚠️  Some tests failed"
    exit 1
fi