--[[
  Quest 858: Clones of Marual
  Level Required: 89
  NPC Start: 126, NPC Complete: 126
  Prerequisites: [857]
  Rewards: EXP=4431094, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_858 = fsm:new({
    id = 858,
    name = "Clones of Marual",
    level_required = 89,
    npc_start = 126,
    npc_complete = 126,
    prerequisites = {857},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 858: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 858: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 858: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 858: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 858: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 858: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 858: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 858: Rewards given")
                self:give_rewards({
                    exp = 4431094,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_858