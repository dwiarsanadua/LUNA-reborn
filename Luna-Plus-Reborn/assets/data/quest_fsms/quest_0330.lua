--[[
  Quest 330: [B Class] Wind Basilisk Elimination
  Level Required: 51
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [329]
  Rewards: EXP=317002
]]

local fsm = require('fsm_engine')

local quest_330 = fsm:new({
    id = 330,
    name = "[B Class] Wind Basilisk Elimination",
    level_required = 51,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {329},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 330: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 330: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 330: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 92,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 330: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 330: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 330: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 330: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 330: Rewards given")
                self:give_rewards({
                    exp = 317002,
                    items = {
                        { item_id = 236, count = 1 },
                        { item_id = 236, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_330