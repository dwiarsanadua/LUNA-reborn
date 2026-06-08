--[[
  Quest 908: [Party]Pollen Allergy
  Level Required: 81
  NPC Start: 98, NPC Complete: 98
  Rewards: EXP=2266495
]]

local fsm = require('fsm_engine')

local quest_908 = fsm:new({
    id = 908,
    name = "[Party]Pollen Allergy",
    level_required = 81,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 908: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 908: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 908: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 273,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 908: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 276,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 908: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 908: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 908: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 908: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 908: Rewards given")
                self:give_rewards({
                    exp = 2266495,
                })
            end,
            transitions = {},
        },
    },
})

return quest_908