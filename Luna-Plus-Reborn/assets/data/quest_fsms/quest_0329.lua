--[[
  Quest 329: [B Class] Wind Basilisk Elimination
  Level Required: 51
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [327]
  Rewards: EXP=317002
]]

local fsm = require('fsm_engine')

local quest_329 = fsm:new({
    id = 329,
    name = "[B Class] Wind Basilisk Elimination",
    level_required = 51,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {327},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 329: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 329: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 329: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 92,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 329: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 329: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 329: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 329: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 329: Rewards given")
                self:give_rewards({
                    exp = 317002,
                    items = {
                        { item_id = 235, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_329