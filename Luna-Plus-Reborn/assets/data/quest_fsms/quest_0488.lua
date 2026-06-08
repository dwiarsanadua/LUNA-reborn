--[[
  Quest 488: [Special][Quest] Decent Fisherman
  Level Required: 0
  NPC Start: 64, NPC Complete: 121
  Prerequisites: [487]
  Rewards: EXP=93
]]

local fsm = require('fsm_engine')

local quest_488 = fsm:new({
    id = 488,
    name = "[Special][Quest] Decent Fisherman",
    level_required = 0,
    npc_start = 64,
    npc_complete = 121,
    prerequisites = {487},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 488: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 488: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 488: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 488: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 488: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 488: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 488: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 488: Rewards given")
                self:give_rewards({
                    exp = 93,
                    items = {
                        { item_id = 21000317, count = 50 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_488