loadz   lw      0       1       zero    # load 0 into reg1      (0)
loadm   lw      1       2       neg1    # load -1 into reg2     (1)
loadps  lw      1       4       pos1    # load 1 into reg5      (2)
loadc   lw      1       0       mcand   # load mcand into reg0  (3)
loadp   lw      1       3       mpiler  # load mpiler into reg3 (4)
judge   beq     0       6       done    # reg0 == 0 -> halt     (5)
addm1   add     0       2       0       # reg0--                (6)
addc    add     1       3       1       # reg1 += reg3          (7)
back    beq     6       6       -4      # jump to judge         (8)
done    halt                            # end                   (9)
mcand   .fill   100
mpiler  .fill   10
neg1    .fill   -1
pos1    .fill   1
zero    .fill   0
