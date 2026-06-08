--[[
  Quest 756: [Quest] Majin's Blessing
  Level Required: 0
  NPC Start: 400, NPC Complete: 402
  Prerequisites: [755]
]]

local fsm = require('fsm_engine')

local quest_756 = fsm:new({
    id = 756,
    name = "[Quest] Majin's Blessing",
    level_required = 0,
    npc_start = 400,
    npc_complete = 402,
    prerequisites = {755},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 756: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 756: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 756: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 756: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 756: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 756: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 402,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 756: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 756: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 12009440, count = 1 },
                        { item_id = 21000004, count = 50 },
                        { item_id = 21000009, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_756