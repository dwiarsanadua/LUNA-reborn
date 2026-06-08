--[[
  Quest 918: [Party] Together than Alone, Right!
  Level Required: 83
  NPC Start: 123, NPC Complete: 123
  Rewards: EXP=2717141
]]

local fsm = require('fsm_engine')

local quest_918 = fsm:new({
    id = 918,
    name = "[Party] Together than Alone, Right!",
    level_required = 83,
    npc_start = 123,
    npc_complete = 123,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 918: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 918: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 918: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 272,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 918: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 277,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 918: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 918: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 918: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 918: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 918: Rewards given")
                self:give_rewards({
                    exp = 2717141,
                })
            end,
            transitions = {},
        },
    },
})

return quest_918