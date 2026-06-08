--[[
  Quest 39: Dark Zombie
  Level Required: 51
  NPC Start: 39, NPC Complete: 39
  Rewards: EXP=164841, Gold=6320
]]

local fsm = require('fsm_engine')

local quest_39 = fsm:new({
    id = 39,
    name = "Dark Zombie",
    level_required = 51,
    npc_start = 39,
    npc_complete = 39,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 39: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 39: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 39: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 39: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 39: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 39: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 39: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 39: Rewards given")
                self:give_rewards({
                    exp = 164841,
                    gold = 6320,
                })
            end,
            transitions = {},
        },
    },
})

return quest_39