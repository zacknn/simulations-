### Floating Point Problem :
    in my code . when the ball stop bouncing , the print say a deferent thing , is start to show many many values as if the ball never stop bouncing , when i search for it , i found that problem caled `Floating Problem` it a classic physics problem , when the ball looks like it has stopped , the math say a deferent story :
Why this happens

In my code, i have a line like this: `velocity.y *= -0.7f;`.

    When the ball hits the ground, the velocity gets smaller (e.g., 10→7→4.9→3.4).

    Eventually, the velocity becomes something tiny like 0.000001.

    To the computer, 0.000001 is not zero.

    Gravity is still pulling the ball down every single frame, and the collision code sees that the ball is "below" the floor, so it keeps "bouncing" it by a microscopic amount.

The ball is essentially vibrating at a sub-pixel level that our eye can't see, but my if (position.y >= 580) check is still true, so it prints to the terminal infinitely.
