--[[
  Quest 788: [Party] Board of Giants
  Level Required: 49
  NPC Start: 62, NPC Complete: 62
  Rewards: EXP=193736, Gold=7415
]]

local fsm = require('fsm_engine')

local quest_788 = fsm:new({
    id = 788,
    name = "[Party] Board of Giants",
    level_required = 49,
    npc_start = 62,
    npc_complete = 62,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 788: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 788: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 788: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 39,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 788: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 788: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 788: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 788: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 788: Rewards given")
                self:give_rewards({
                    exp = 193736,
                    gold = 7415,
                })
            end,
            transitions = {},
        },
    },
})

return quest_788