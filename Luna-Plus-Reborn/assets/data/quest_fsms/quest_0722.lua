--[[
  Quest 722: Ghost Tree
  Level Required: 60
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=437964
]]

local fsm = require('fsm_engine')

local quest_722 = fsm:new({
    id = 722,
    name = "Ghost Tree",
    level_required = 60,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 722: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 722: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 722: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 396,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 722: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 722: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 722: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 722: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 722: Rewards given")
                self:give_rewards({
                    exp = 437964,
                })
            end,
            transitions = {},
        },
    },
})

return quest_722