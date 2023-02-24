<script lang="ts">
    import type { LayoutData } from './$types';
    export let data: LayoutData;
    import Navbar from '../components/navbar.svelte';
    
</script>
<Navbar />
<slot />