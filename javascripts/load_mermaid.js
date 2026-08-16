// Dynamically load Mermaid ES module and initialize it
import('https://unpkg.com/mermaid@10.8.0/dist/mermaid.esm.min.mjs').then(mermaidModule => {
    const mermaid = mermaidModule.default;
    mermaid.initialize({ startOnLoad: false, theme: "default" });
    
    function renderMermaid() {
        // Material for MkDocs converts fence_code_format into <pre><code class="mermaid">
        // Or if fence_div_format is used, it converts to <div class="mermaid">
        // Mermaid expects the content to be directly inside the queried element.
        // If it's <pre><code class="mermaid">, we might need to handle it.
        const codeBlocks = document.querySelectorAll('pre.mermaid > code');
        codeBlocks.forEach(block => {
            const pre = block.parentElement;
            const div = document.createElement('div');
            div.className = 'mermaid';
            div.textContent = block.textContent;
            pre.replaceWith(div);
        });

        const blocks = document.querySelectorAll('.mermaid');
        if (blocks.length > 0) {
            mermaid.run({ querySelector: '.mermaid' }).catch(e => console.error(e));
        }
    }

    // Run initially
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', renderMermaid);
    } else {
        renderMermaid();
    }

    // Run on MkDocs Material instant navigation
    document.body.addEventListener("DOMContentSwitch", function() {
        renderMermaid();
    });
}).catch(err => console.error("Failed to load Mermaid module:", err));
