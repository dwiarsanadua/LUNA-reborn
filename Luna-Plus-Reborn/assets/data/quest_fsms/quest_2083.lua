--[[
  Quest 2083: The Magic
  Level Required: 75
  NPC Start: 9, NPC Complete: 71
  Prerequisites: [282]
]]

local fsm = require('fsm_engine')

local quest_2083 = fsm:new({
    id = 2083,
    name = "The Magic",
    level_required = 75,
    npc_start = 9,
    npc_complete = 71,
    prerequisites = {282},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2083: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2083: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2083: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2083: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2083: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2083: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2083: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 71,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2083: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2083: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2083: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 71,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2083: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2083: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 189, count = 1 },
                        { item_id = 189, count = 1 },
                        { item_id = 190, count = 1 },
                        { item_id = 190, count = 1 },
                        { item_id = 190, count = 25 },
                        { item_id = 191, count = 1 },
                        { item_id = 191, count = 1 },
                        { item_id = 192, count = 1 },
                        { item_id = 192, count = 1 },
                        { item_id = 30000484, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2083