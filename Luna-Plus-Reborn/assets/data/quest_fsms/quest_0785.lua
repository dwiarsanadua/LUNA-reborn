--[[
  Quest 785: [Party] Cocatris
  Level Required: 47
  NPC Start: 62, NPC Complete: 62
  Rewards: EXP=166149, Gold=7415
]]

local fsm = require('fsm_engine')

local quest_785 = fsm:new({
    id = 785,
    name = "[Party] Cocatris",
    level_required = 47,
    npc_start = 62,
    npc_complete = 62,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 785: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 785: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 785: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 145,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 785: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 785: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 785: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 785: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 785: Rewards given")
                self:give_rewards({
                    exp = 166149,
                    gold = 7415,
                })
            end,
            transitions = {},
        },
    },
})

return quest_785