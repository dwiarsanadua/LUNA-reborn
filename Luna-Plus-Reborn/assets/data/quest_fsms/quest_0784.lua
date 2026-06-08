--[[
  Quest 784: Entrance to the Ravine
  Level Required: 47
  NPC Start: 62, NPC Complete: 62
  Rewards: EXP=138458, Gold=7415
]]

local fsm = require('fsm_engine')

local quest_784 = fsm:new({
    id = 784,
    name = "Entrance to the Ravine",
    level_required = 47,
    npc_start = 62,
    npc_complete = 62,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 784: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 784: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 784: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 784: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 784: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 784: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 784: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 784: Rewards given")
                self:give_rewards({
                    exp = 138458,
                    gold = 7415,
                })
            end,
            transitions = {},
        },
    },
})

return quest_784