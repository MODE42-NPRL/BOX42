int perm_can_delete_user(int actor, int target)
{
    if (actor == 4) {
        if (target == 4 && actor != target)
            return 0;               /* Owner darf anderen Owner NICHT löschen */
        return 1;                   /* Owner darf sich selbst und alle anderen löschen */
    }

    if (actor == 3 && target <= 2)
        return 1;                   /* Admin darf User & Mods löschen */

    return 0;
}

int perm_can_edit_user(int actor, int target)
{
    if (actor == 4) {
        if (target == 4 && actor != target)
            return 0;               /* Owner darf anderen Owner NICHT bearbeiten */
        return 1;                   /* Owner darf sich selbst und alle anderen bearbeiten */
    }

    if (actor == 3 && target <= 2)
        return 1;                   /* Admin darf User & Mods bearbeiten */

    return 0;
}
