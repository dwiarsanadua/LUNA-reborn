--[[
  Quest 857: I know a woman's revenge
  Level Required: 89
  NPC Start: 129, NPC Complete: 129
  Rewards: EXP=4431094, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_857 = fsm:new({
    id = 857,
    name = "I know a woman's revenge",
    level_required = 89,
    npc_start = 129,
    npc_complete = 129,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 857: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 857: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 857: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 857: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 857: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 857: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 857: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 857: Rewards given")
                self:give_rewards({
                    exp = 4431094,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_857