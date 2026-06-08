--[[
  Quest 723: [Party] The Old Haunted Tree
  Level Required: 90
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [722]
  Rewards: EXP=4122943
]]

local fsm = require('fsm_engine')

local quest_723 = fsm:new({
    id = 723,
    name = "[Party] The Old Haunted Tree",
    level_required = 90,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {722},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 723: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 723: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 723: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 640,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 723: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 723: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 723: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 723: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 723: Rewards given")
                self:give_rewards({
                    exp = 4122943,
                })
            end,
            transitions = {},
        },
    },
})

return quest_723