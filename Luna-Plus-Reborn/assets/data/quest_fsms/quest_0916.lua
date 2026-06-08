--[[
  Quest 916: Serie's Tears
  Level Required: 83
  NPC Start: 124, NPC Complete: 124
  Rewards: EXP=2717141
]]

local fsm = require('fsm_engine')

local quest_916 = fsm:new({
    id = 916,
    name = "Serie's Tears",
    level_required = 83,
    npc_start = 124,
    npc_complete = 124,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 916: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 916: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 916: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 278,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 916: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 916: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 916: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 916: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 916: Rewards given")
                self:give_rewards({
                    exp = 2717141,
                })
            end,
            transitions = {},
        },
    },
})

return quest_916