--[[
  Quest 489: [Special][Quest] 3 Common Carps
  Level Required: 0
  NPC Start: 64, NPC Complete: 121
  Prerequisites: [488]
  Rewards: EXP=237
]]

local fsm = require('fsm_engine')

local quest_489 = fsm:new({
    id = 489,
    name = "[Special][Quest] 3 Common Carps",
    level_required = 0,
    npc_start = 64,
    npc_complete = 121,
    prerequisites = {488},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 489: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 489: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 489: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 489: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 489: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 489: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 121,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 489: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 489: Rewards given")
                self:give_rewards({
                    exp = 237,
                    items = {
                        { item_id = 21000308, count = 3 },
                        { item_id = 21000317, count = 50 },
                        { item_id = 21000009, count = 6 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_489