--[[
  Quest 744: [Quest] Thin day after the fair
  Level Required: 66
  NPC Start: 126, NPC Complete: 126
  Prerequisites: [743]
  Rewards: EXP=1117488
]]

local fsm = require('fsm_engine')

local quest_744 = fsm:new({
    id = 744,
    name = "[Quest] Thin day after the fair",
    level_required = 66,
    npc_start = 126,
    npc_complete = 126,
    prerequisites = {743},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 744: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 744: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 744: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 744: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 641,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 744: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 744: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 744: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 744: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 744: Rewards given")
                self:give_rewards({
                    exp = 1117488,
                    items = {
                        { item_id = 366, count = 30 },
                        { item_id = 367, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_744